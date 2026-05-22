const mqtt = require("mqtt");

const client = mqtt.connect("mqtt://192.168.1.55:1883");

client.on("connect", () => {
  console.log("Client MQTT connecté pour la simulation de données de capteurs");
  startSimulation();
});

client.on("error", (error) => {
  console.error("Erreur MQTT:", error);
  process.exit(1);
});

const waterLevelStates = ["empty", "critical", "low", "medium", "full"];

function simulateSensorData() {
  client.publish(
    "florapot/sensors",
    JSON.stringify({
      soil_moisture: Math.floor(Math.random() * 101),
      light: Math.floor(Math.random() * 101),
      water_level_state: waterLevelStates[Math.floor(Math.random() * waterLevelStates.length)],
      pump_state: Math.random() < 0.5 ? true : false,
    }),
  );
}

function startSimulation() {
  setInterval(() => {
    console.log("Simulation de données de capteurs...");
    simulateSensorData();
  }, 5000);
}