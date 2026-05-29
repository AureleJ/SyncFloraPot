const mqtt = require("mqtt");
const { Server } = require("socket.io");
const http = require("http");

const server = http.createServer();

const io = new Server(server, {
  cors: {
    origin: "*",
    methods: ["GET", "POST"],
  },
});

const mqttUrl = process.env.MQTT_URL || "mqtt://mqtt:1883";
const mqttClient = mqtt.connect(mqttUrl);
const topic = "florapot/sensors";

mqttClient.on("connect", () => {
  console.log(`Client MQTT connected`);
  mqttClient.subscribe(topic);
});

mqttClient.on("message", (topic, message) => {
  console.log(`Message reçu - Topic: ${topic}, Message: ${message}`);

  const payload = JSON.parse(message.toString());

  io.emit("syncFloraPotData", payload);
});

mqttClient.on("error", (error) => {
  console.error("Erreur:", error);
  process.exit(1);
});

io.on("connection", (socket) => {
  console.log(`Client websocket connected: ${socket.id}`);

  socket.on("disconnect", () => {
    console.log(`Client websocket disconnected: ${socket.id}`);
  });
});

const port = process.env.PORT || 3000;
server.listen(port, () => console.log(`Listening on port ${port}`));