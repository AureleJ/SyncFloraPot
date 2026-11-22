require("dotenv").config();
const mqtt = require("mqtt");
const { Server } = require("socket.io");
const http = require("http");
const { Pool } = require("pg");
const express = require("express");
const jwt = require("jsonwebtoken");
const bcrypt = require("bcrypt");
const cors = require("cors");

const pool = new Pool({
  user: process.env.POSTGRES_USER,
  password: process.env.POSTGRES_PASSWORD,
  host: process.env.POSTGRES_HOST,
  port: process.env.POSTGRES_PORT,
  database: process.env.POSTGRES_DB,
});

const app = express();
app.use(express.json());
app.use(
  cors({
    origin: "*",
    methods: ["GET", "POST"],
    allowedHeaders: ["Content-Type", "Authorization"],
  })
);

const server = http.createServer(app);

/* Authentication Middleware */
const JWT_SECRET = process.env.JWT_SECRET;

const authenticateToken = (req, res, next) => {
  const authHeader = req.headers["authorization"];
  const token = authHeader && authHeader.split(" ")[1];

  if (!token) {
    return res.status(401).json({ message: "Access token required" });
  }

  try {
    const decoded = jwt.verify(token, JWT_SECRET);
    req.user = decoded;
    next();
  } catch (error) {
    return res.status(403).json({ message: "Invalid or expired token" });
  }
};

/* WebSocket Setup */
const io = new Server(server, {
  cors: {
    origin: "*",
    methods: ["GET", "POST"],
  },
});

io.on("connection", (socket) => {
  const token = socket.handshake.query.token;

  if (!token) {
    socket.disconnect();
    return;
  }

  try {
    const decoded = jwt.verify(token, JWT_SECRET);
    const userId = decoded.id;

    socket.join(`user:${userId}`);
    console.log(`User ${userId} joined room user:${userId}`);

    socket.on("disconnect", () => {
      console.log(`User ${userId} disconnected`);
    });
  } catch (error) {
    socket.disconnect();
  }
});

/* MQTT Client Setup */
const mqttUrl = process.env.MQTT_URL;
const mqttClient = mqtt.connect(mqttUrl);
const topicWildcard = "syncflorapot/pots/+";

mqttClient.on("connect", () => {
  console.log("Client MQTT connected");
  mqttClient.subscribe(topicWildcard);
});

mqttClient.on("message", async (topic, message) => {
  try {
    const payload = JSON.parse(message.toString());
    const hardwareId = topic.split("/")[2];

    console.log(`Topic : ${topic} Message : ${message} Id : ${hardwareId}`);

    // 1. Trouver le pot par son device_id
    const potResult = await pool.query(
      `SELECT id, user_id FROM pots WHERE device_id = $1`,
      [hardwareId]
    );
    console.log(`potResult : ${potResult}`);

    if (potResult.rows.length === 0) {
      console.warn(`Aucun pot enregistré avec device_id: ${hardwareId}`);
      return;
    }

    const pot = potResult.rows[0];

    // 2. Insérer dans metrics (cast booléen explicite pour pump_state)
    await pool.query(
      `INSERT INTO metrics (time, pot_id, soil_moisture, light, water_level, pump_state)
       VALUES (NOW(), $1, $2, $3, $4, $5)`,
      [
        pot.id,
        payload.soil_moisture ?? null,
        payload.light ?? null,
        payload.water_level ?? null,
        Boolean(payload.pump_state), // Évite le rejet d'entier 0/1 par Postgres
      ]
    );

    console.log("Ajoute au truc");

    // 3. Diffuser en temps réel au propriétaire
    io.to(`user:${pot.user_id}`).emit("syncFloraPotData", {
      potId: pot.id,
      ...payload,
    });
  } catch (error) {
    console.error("Erreur traitement MQTT :", error.message);
  }
});

mqttClient.on("error", (error) => {
  console.error("Erreur client MQTT :", error);
});

/* API Routes */
// Inscription
app.post("/auth/register", async (req, res) => {
  const { email, password } = req.body;

  if (!email || !password) {
    return res.status(400).json({ message: "Email et mot de passe requis" });
  }

  try {
    const saltRounds = 10;
    const passwordHash = await bcrypt.hash(password, saltRounds);

    const result = await pool.query(
      `INSERT INTO users (email, password_hash) VALUES ($1, $2) RETURNING id, email`,
      [email.toLowerCase().trim(), passwordHash]
    );

    const user = result.rows[0];
    res.status(201).json({ id: user.id, email: user.email });
  } catch (error) {
    if (error.code === "23505") {
      return res.status(409).json({ message: "Cet email est déjà utilisé" });
    }
    console.error("Registration error:", error);
    res.status(500).json({ message: "Internal server error" });
  }
});

// Connexion
app.post("/auth/login", async (req, res) => {
  const { email, password } = req.body;

  try {
    const result = await pool.query(
      `SELECT * FROM users WHERE email = $1`,
      [email.toLowerCase().trim()]
    );

    if (result.rows.length === 0) {
      return res.status(401).json({ message: "Identifiants invalides" });
    }

    const user = result.rows[0];
    const passwordMatch = await bcrypt.compare(password, user.password_hash);

    if (!passwordMatch) {
      return res.status(401).json({ message: "Identifiants invalides" });
    }

    const token = jwt.sign(
      { id: user.id, email: user.email },
      JWT_SECRET,
      { expiresIn: "30d" }
    );

    res.json({ token, user: { id: user.id, email: user.email } });
  } catch (error) {
    console.error("Login error:", error);
    res.status(500).json({ message: "Internal server error" });
  }
});

// Récupération de l'utilisateur courant (Route manquante)
app.get("/auth/me", authenticateToken, async (req, res) => {
  try {
    const result = await pool.query(
      `SELECT id, email FROM users WHERE id = $1`,
      [req.user.id]
    );

    if (result.rows.length === 0) {
      return res.status(404).json({ message: "Utilisateur non trouvé" });
    }

    res.json(result.rows[0]);
  } catch (error) {
    res.status(500).json({ message: "Internal server error" });
  }
});

// Liste des pots de l'utilisateur
app.get("/pots", authenticateToken, async (req, res) => {
  try {
    const result = await pool.query(
      `SELECT p.id, p.name, p.location, p.species, p.device_id,
       MAX(m.time) AS last_seen
       FROM pots p
       LEFT JOIN metrics m ON m.pot_id = p.id
       WHERE p.user_id = $1
       GROUP BY p.id
       ORDER BY p.created_at DESC`,
      [req.user.id]
    );

    const pots = result.rows.map((pot) => ({
      ...pot,
      connected: pot.last_seen
        ? new Date() - new Date(pot.last_seen) < 120000
        : false,
    }));

    res.json(pots);
  } catch (error) {
    console.error("Error fetching pots:", error);
    res.status(500).json({ message: "Internal server error" });
  }
});

// Création d'un pot (avec device_id et species)
app.post("/pots", authenticateToken, async (req, res) => {
  const { name, location, species, deviceId } = req.body;

  if (!name || !location || !species || !deviceId) {
    return res.status(400).json({
      message: "Champs requis manquants: name, location, species, deviceId",
    });
  }

  try {
    const result = await pool.query(
      `INSERT INTO pots (name, location, species, device_id, user_id)
       VALUES ($1, $2, $3, $4, $5)
       RETURNING *`,
      [name.trim(), location.trim(), species.trim(), deviceId.trim(), req.user.id]
    );

    res.status(201).json(result.rows[0]);
  } catch (error) {
    if (error.code === "23505") {
      return res.status(409).json({ message: "Ce pot (device_id) est déjà associé." });
    }
    console.error("Error adding pot:", error);
    res.status(500).json({ message: "Internal server error" });
  }
});

// Supprimer un pot
app.delete("/pots/:id", authenticateToken, async (req, res) => {
  const potId = req.params.id;

  try {
    await pool.query(`DELETE FROM metrics WHERE pot_id = $1`, [potId]);

    const result = await pool.query(
      `DELETE FROM pots WHERE id = $1 AND user_id = $2 RETURNING id`,
      [potId, req.user.id]
    );

    if (result.rowCount === 0) {
      return res.status(404).json({ message: "Pot introuvable ou non autorisé" });
    }

    res.json({ message: "Pot supprimé avec succès", id: potId });
  } catch (error) {
    console.error("Error deleting pot:", error);
    res.status(500).json({ message: "Internal server error" });
  }
});

// Modifier un pot
app.put("/pots/:id", authenticateToken, async (req, res) => {
  const potId = req.params.id;
  const { name, location, species } = req.body;

  if (!name && !location && !species) {
    return res.status(400).json({
      message: "Au moins un champ requis à modifier : name, location ou species",
    });
  }

  try {
    const result = await pool.query(
      `UPDATE pots
       SET name = COALESCE($1, name),
           location = COALESCE($2, location),
           species = COALESCE($3, species)
       WHERE id = $4 AND user_id = $5
       RETURNING id, name, location, species, device_id, created_at`,
      [
        name ? name.trim() : null,
        location ? location.trim() : null,
        species ? species.trim() : null,
        potId,
        req.user.id,
      ]
    );

    if (result.rowCount === 0) {
      return res.status(404).json({ message: "Pot introuvable ou non autorisé" });
    }

    res.json(result.rows[0]);
  } catch (error) {
    console.error("Error updating pot:", error);
    res.status(500).json({ message: "Internal server error" });
  }
});

// Récupération de la dernière métrique pour un pot
app.get("/pots/:id/latest", authenticateToken, async (req, res) => {
  try {
    const result = await pool.query(
      `SELECT m.* 
       FROM metrics m
       JOIN pots p ON p.id = m.pot_id
       WHERE m.pot_id = $1 AND p.user_id = $2
       ORDER BY m.time DESC 
       LIMIT 1`,
      [req.params.id, req.user.id]
    );

    if (result.rows.length === 0) {
      return res.json(null);
    }

    res.json(result.rows[0]);
  } catch (error) {
    console.error("Error fetching latest metrics:", error);
    res.status(500).json({ message: "Internal server error" });
  }
});

const port = process.env.PORT;
server.listen(port, () => console.log(`Listening on port ${port}`));
