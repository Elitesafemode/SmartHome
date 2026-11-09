const express = require("express");
const mqtt = require("mqtt");
const path = require("path");
const crypto = require("crypto");

const app = express();
app.use(express.json());
app.use(express.static(path.join(__dirname, "public")));

const MQTT_URL =
  process.env.MQTT_URL || "mqtt://mosquitto-lyo-smarthome.runflare.cloud:1883";
const MQTT_USER = process.env.MQTT_USER || "mosquitto";
const MQTT_PASS = process.env.MQTT_PASS || "fPViRAJDnqzluin35M6v";

const TOPIC_REQ = "smarthome/api/request";
const TOPIC_RES = "smarthome/api/response";

const pendingRequests = new Map();

const client = mqtt.connect(MQTT_URL, {
  username: MQTT_USER,
  password: MQTT_PASS,
  reconnectPeriod: 3000,
});

client.on("connect", () => {
  console.log("Connected to Mosquitto broker");
  client.subscribe(TOPIC_RES);
});

client.on("message", (topic, message) => {
  if (topic !== TOPIC_RES) return;
  try {
    const data = JSON.parse(message.toString());
    const pending = pendingRequests.get(data.reqId);
    if (pending) {
      clearTimeout(pending.timer);
      pendingRequests.delete(data.reqId);
      pending.res
        .status(data.status || 200)
        .type("application/json")
        .send(data.payload || "{}");
    }
  } catch (err) {
    console.error("Invalid MQTT response", err);
  }
});

app.all("/api/*", (req, res) => {
  if (!client.connected) {
    return res
      .status(503)
      .json({ success: false, message: "عدم اتصال سرور به بروکر MQTT" });
  }

  const reqId = crypto.randomUUID();
  const authHeader = req.headers["authorization"] || "";
  const token = authHeader.startsWith("Bearer ") ? authHeader.substring(7) : "";

  const timer = setTimeout(() => {
    pendingRequests.delete(reqId);
    res
      .status(504)
      .json({
        success: false,
        message: "برد پاسخ نداد اتصال اینترنت ESP32 را بررسی کنید",
      });
  }, 6000);

  pendingRequests.set(reqId, { res, timer });

  const payload = JSON.stringify({
    reqId,
    path: req.path,
    method: req.method,
    token,
    body: req.body || null,
  });

  client.publish(TOPIC_REQ, payload);
});

const PORT = process.env.PORT || 3000;
app.listen(PORT, "0.0.0.0", () => {
  console.log(`SmartHome Web Panel running on port ${PORT}`);
});
