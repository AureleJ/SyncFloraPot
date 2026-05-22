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

const mqttClient = mqtt.connect("mqtt://192.168.1.55:1883");
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

server.listen(3000, () => console.log("Listening on port 3000"));