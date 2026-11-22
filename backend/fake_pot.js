require("dotenv").config();
const mqtt = require("mqtt");
const potId = "PROV_543204";
const potPassword = process.env.FAKE_POT_PASSWORD;
const topic = `syncflorapot/pots/${potId}`;

for (const v of ["MQTT_WSS_URL", "MQTT_USERNAME", "MQTT_PASSWORD"]) {
  if (!process.env[v]) {
    console.error(`Variable d'environnement manquante: ${v} (copier .env.example en .env)`);
    process.exit(1);
  }
}

const client = mqtt.connect(process.env.MQTT_WSS_URL, {
  username: process.env.MQTT_USERNAME,
  password: process.env.MQTT_PASSWORD,
  rejectUnauthorized: true,
  reconnectPeriod: 3000,
});

client.on("connect", () => {
  console.log("Client MQTT authentifié et connecté (simulation active)");
  startSimulation();
});

client.on("error", (error) => {
  console.error("Erreur MQTT :", error.message);
});

function simulateSensorData() {
  const payload = {
    soil_moisture: Math.floor(Math.random() * 101),
    light: Math.floor(Math.random() * 101),
    water_level: Math.floor(Math.random() * 101),
    pump_state: Math.random() < 0.5,
  };

  client.publish(topic, JSON.stringify(payload), (err) => {
    if (err) {
      console.error("Échec publication (ACL ou réseau) :", err);
    } else {
      console.log(`[\({new Date().toLocaleTimeString()}] Données envoyées sur\){topic} :`, payload);
    }
  });
}

function startSimulation() {
  simulateSensorData();
  setInterval(simulateSensorData, 1000);
}