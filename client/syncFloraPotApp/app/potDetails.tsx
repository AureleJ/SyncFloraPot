import { MaterialCommunityIcons } from "@expo/vector-icons";
import { useLocalSearchParams, useRouter } from "expo-router";
import { StatusBar } from "expo-status-bar";
import React, { useEffect, useState } from "react";
import { Pressable, ScrollView, StyleSheet, Text, View } from "react-native";
import { io, Socket } from "socket.io-client";

import Background from "../components/Background";
import { appStyles } from "../styles/appStyles";
import { colors } from "../styles/colors";
import {
  borderRadius,
  fontSizes,
  fontWeights,
  spacing,
} from "../styles/spacing";

interface RawSensorData {
  soil_moisture: number;
  light: number;
  water_level_state: string;
  pump_state: boolean;
}

interface SensorThresholds {
  min: number;
  max: number;
  warningMin: number;
  warningMax: number;
}

const SENSOR_THRESHOLDS: Record<string, SensorThresholds> = {
  soil_moisture: { min: 20, max: 80, warningMin: 15, warningMax: 90 },
  light: { min: 10, max: 100, warningMin: 5, warningMax: 120 },
  water_level_state: { min: 0, max: 5, warningMin: 0, warningMax: 5 },
  pump_state: { min: 0, max: 1, warningMin: 0, warningMax: 1 },
};

interface SensorDisplay {
  title: string;
  value: string | number;
}

interface FormattedSensorData {
  soil_moisture: SensorDisplay;
  light: SensorDisplay;
  water_level_state: SensorDisplay;
  pump_state: SensorDisplay;
}

const emptySensorData: FormattedSensorData = {
  soil_moisture: {
    title: "Soil Moisture",
    value: "--",
  },
  light: {
    title: "Light Intensity",
    value: "--",
  },
  water_level_state: {
    title: "Water Level",
    value: "--",
  },
  pump_state: {
    title: "Pump State",
    value: "--",
  },
};

function PotDetails() {
  const router = useRouter();
  const params = useLocalSearchParams<{ potId?: string; potName?: string }>();

  const [data, setData] = useState<FormattedSensorData | null>(null);
  const [isConnected, setIsConnected] = useState<boolean>(false);
  const [lastUpdate, setLastUpdate] = useState<number | null>(null);
  const displayedData = data ?? emptySensorData;

  useEffect(() => {
    const socket: Socket = io("http://192.168.1.55:3000");

    socket.on("connect", () => {
      console.log("Connected to SyncFloraPot server");
      setIsConnected(true);
    });

    socket.on("syncFloraPotData", (incomingData: RawSensorData) => {
      try {
        console.log("Sensor data received:", incomingData);
        setData(convertData(incomingData));
        setLastUpdate(Date.now());
      } catch (error) {
        console.error("Data conversion error:", error);
        console.error("Raw data was:", incomingData);
      }
    });

    socket.on("disconnect", () => {
      setIsConnected(false);
    });

    return () => {
      socket.disconnect();
    };
  }, []);

  return (
    <Background>
      <View style={appStyles.mainContainer}>
        <StatusBar style="light" translucent backgroundColor="transparent" />

        <ScrollView contentContainerStyle={appStyles.scrollContainer}>
          <View style={appStyles.topBar}>
            <Pressable
              onPress={() => router.back()}
              style={({ pressed }) => [
                styles.backButton,
                pressed && styles.backButtonPressed,
              ]}
            >
              <MaterialCommunityIcons
                name="chevron-left"
                size={18}
                color={colors.textPrimary}
              />
              <Text style={styles.backLabel}>Back</Text>
            </Pressable>

            <View
              style={[
                appStyles.statusBadge,
                isConnected
                  ? appStyles.statusConnected
                  : appStyles.statusDisconnected,
              ]}
            >
              <MaterialCommunityIcons
                name={isConnected ? "wifi" : "wifi-off"}
                size={14}
                color={colors.textPrimary}
              />
              <Text style={appStyles.statusText}>
                {isConnected ? "Connected" : "Disconnected"}
              </Text>
            </View>
          </View>

          <View style={styles.headerRow}>
            <View>
              <Text style={appStyles.heading}>
                {params.potName ? String(params.potName) : "SyncFlora"}
              </Text>
            </View>
            <Pressable
              onPress={() => setLastUpdate(Date.now())}
              style={({ pressed }) => [
                styles.refreshButton,
                pressed && styles.refreshButtonPressed,
              ]}
            >
              <MaterialCommunityIcons
                name="refresh"
                size={20}
                color={colors.primary}
              />
            </Pressable>
          </View>
          {lastUpdate && (
            <Text style={styles.lastUpdateText}>
              Last update: {formatTime(lastUpdate)}
            </Text>
          )}

          <View style={styles.metricsList}>
            {Object.entries(displayedData).map(([key, sensor]) => {
              const healthStatus = getHealthStatus(key, sensor.value);
              return (
                <View key={key} style={styles.metricRow}>
                  <View style={styles.metricIconWrapper}>
                    <MaterialCommunityIcons
                      name={getSensorIcon(key) as any}
                      size={24}
                      color={getSensorColor(key)}
                    />
                  </View>
                  <View style={styles.metricContent}>
                    <View style={styles.metricHeader}>
                      <Text style={styles.metricLabel}>{sensor.title}</Text>
                      <View
                        style={[
                          styles.healthBadge,
                          { backgroundColor: getHealthColor(healthStatus) },
                        ]}
                      >
                        <Text style={styles.healthBadgeText}>
                          {healthStatus === "healthy"
                            ? "✓"
                            : healthStatus === "warning"
                              ? "!"
                              : "⚠"}
                        </Text>
                      </View>
                    </View>
                    <Text style={styles.metricValueLarge}>{sensor.value}</Text>
                  </View>
                </View>
              );
            })}
          </View>
        </ScrollView>
      </View>
    </Background>
  );
}

function getHealthStatus(
  sensorKey: string,
  value: string | number,
): "healthy" | "warning" | "critical" {
  if (value === "--") return "critical";

  const numValue =
    typeof value === "string"
      ? parseFloat(value.replace(/[^0-9.]/g, ""))
      : value;
  const thresholds = SENSOR_THRESHOLDS[sensorKey];

  if (!thresholds) return "healthy";

  if (numValue < thresholds.warningMin || numValue > thresholds.warningMax) {
    return "critical";
  }
  if (numValue < thresholds.min || numValue > thresholds.max) {
    return "warning";
  }
  return "healthy";
}

function getHealthColor(status: "healthy" | "warning" | "critical"): string {
  switch (status) {
    case "healthy":
      return colors.success;
    case "warning":
      return colors.warning;
    case "critical":
      return colors.error;
  }
}

function formatTime(timestamp: number): string {
  const now = Date.now();
  const diff = Math.floor((now - timestamp) / 1000);

  if (diff < 60) return "just now";
  if (diff < 3600) return `${Math.floor(diff / 60)}m ago`;
  if (diff < 86400) return `${Math.floor(diff / 3600)}h ago`;
  return `${Math.floor(diff / 86400)}d ago`;
}

function getSensorIcon(sensorKey: string): string {
  switch (sensorKey) {
    case "soil_moisture":
      return "water-percent";
    case "light":
      return "white-balance-sunny";
    case "water_level_state":
      return "cup-water";
    case "pump_state":
      return "water-pump";
    default:
      return "gauge";
  }
}

function getSensorColor(sensorKey: string): string {
  switch (sensorKey) {
    case "soil_moisture":
      return colors.moistureColor;
    case "light":
      return colors.lightColor;
    case "water_level_state":
      return colors.waterColor;
    case "pump_state":
      return colors.pumpColor;
    default:
      return colors.accent;
  }
}

function convertData(data: RawSensorData): FormattedSensorData {
  console.log("Converting sensor data:", data);

  return {
    soil_moisture: {
      title: "Soil Moisture",
      value: `${Math.round(data.soil_moisture)}%`,
    },
    light: {
      title: "Light Intensity",
      value: `${Math.round(data.light)}%`,
    },
    water_level_state: {
      title: "Water Level",
      value: data.water_level_state || "unknown",
    },
    pump_state: {
      title: "Pump State",
      value: data.pump_state ? "Active" : "Inactive",
    },
  };
}

const styles = StyleSheet.create({
  headerRow: {
    flexDirection: "row",
    alignItems: "center",
    justifyContent: "space-between",
    marginBottom: spacing.md,
  },

  refreshButton: {
    width: 40,
    height: 40,
    borderRadius: borderRadius.full,
    backgroundColor: colors.glassBgStrong,
    justifyContent: "center",
    alignItems: "center",
  },

  refreshButtonPressed: {
    opacity: 0.7,
  },

  lastUpdateText: {
    fontSize: fontSizes.xs,
    color: colors.textTertiary,
    marginBottom: spacing.lg,
  },

  metricsList: {
    gap: spacing.xl,
  },

  metricRow: {
    flexDirection: "row",
    alignItems: "flex-start",
    gap: spacing.lg,
    paddingVertical: spacing.lg,
    borderBottomWidth: 1,
    borderBottomColor: colors.glassBorder,
  },

  metricIconWrapper: {
    width: 40,
    height: 40,
    borderRadius: borderRadius.md,
    backgroundColor: colors.glassBg,
    justifyContent: "center",
    alignItems: "center",
  },

  metricContent: {
    flex: 1,
    gap: spacing.sm,
  },

  metricHeader: {
    flexDirection: "row",
    alignItems: "center",
    justifyContent: "space-between",
    gap: spacing.md,
  },

  metricLabel: {
    fontSize: fontSizes.sm,
    color: colors.textSecondary,
    flex: 1,
  },

  healthBadge: {
    width: 24,
    height: 24,
    borderRadius: borderRadius.full,
    justifyContent: "center",
    alignItems: "center",
  },

  healthBadgeText: {
    color: colors.textPrimary,
    fontSize: fontSizes.sm,
    fontWeight: fontWeights.bold,
  },

  metricValueLarge: {
    fontSize: fontSizes.xxxl,
    fontWeight: fontWeights.bold,
    color: colors.textPrimary,
  },

  backButton: {
    minHeight: 36,
    borderRadius: borderRadius.full,
    backgroundColor: colors.glassBgStrong,
    borderWidth: 1,
    borderColor: colors.glassBorder,
    paddingHorizontal: spacing.md,
    paddingVertical: spacing.xs,
    flexDirection: "row",
    alignItems: "center",
    gap: spacing.xs,
  },
  backButtonPressed: {
    opacity: 0.88,
  },
  backLabel: {
    color: colors.textPrimary,
    fontWeight: "600",
  },
  titleBlock: {
    marginBottom: spacing.md,
  },
});

export default PotDetails;
