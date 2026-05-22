import { MaterialCommunityIcons } from "@expo/vector-icons";
import { useRouter } from "expo-router";
import { StatusBar } from "expo-status-bar";
import React, { useEffect, useState } from "react";
import { Pressable, ScrollView, StyleSheet, Text, View } from "react-native";
import io from "socket.io-client";

import Background from "../components/Background";
import { usePots } from "../context/PotsContext";
import { appStyles } from "../styles/appStyles";
import { colors } from "../styles/colors";
import {
  borderRadius,
  fontSizes,
  fontWeights,
  spacing,
} from "../styles/spacing";

interface SensorData {
  soil_moisture: number;
  light: number;
  water_level: number;
  pump_state: boolean;
  timestamp: number;
}

interface PotSensorState {
  [potId: string]: {
    data?: SensorData;
    lastUpdate?: number;
  };
}

export default function App() {
  const router = useRouter();
  const { pots } = usePots();
  const [sensorData, setSensorData] = useState<PotSensorState>({});

  useEffect(() => {
    const socket = io("http://192.168.1.55:3000");

    socket.on("connect", () => {
      console.log("Connected to sensor server");
    });

    socket.on("syncFloraPotData", (data: any) => {
      setSensorData((prev) => ({
        ...prev,
        [data.potId]: {
          data: data,
          lastUpdate: Date.now(),
        },
      }));
    });

    return () => {
      socket.disconnect();
    };
  }, []);

  const getMoistureStatus = (
    moisture: number,
  ): "healthy" | "warning" | "critical" => {
    if (moisture < 20) return "critical";
    if (moisture < 35) return "warning";
    return "healthy";
  };

  const getStatusColor = (
    status: "healthy" | "warning" | "critical",
  ): string => {
    switch (status) {
      case "healthy":
        return colors.success;
      case "warning":
        return colors.warning;
      case "critical":
        return colors.error;
    }
  };

  const formatTime = (timestamp: number): string => {
    const now = Date.now();
    const diff = Math.floor((now - timestamp) / 1000);
    if (diff < 60) return "now";
    if (diff < 3600) return `${Math.floor(diff / 60)}m`;
    if (diff < 86400) return `${Math.floor(diff / 3600)}h`;
    return `${Math.floor(diff / 86400)}d`;
  };

  return (
    <Background>
      <View style={appStyles.mainContainer}>
        <StatusBar style="light" translucent backgroundColor="transparent" />

        <ScrollView contentContainerStyle={appStyles.scrollContainer}>
          <View style={appStyles.topBar}>
            <Text style={appStyles.heading}>SyncFlora</Text>

            <Pressable
              onPress={() => {
                router.replace("/login");
              }}
            >
              <MaterialCommunityIcons
                name="logout"
                size={20}
                color={colors.textPrimary}
              />
              <Text>Logout</Text>
            </Pressable>

            <Pressable
              accessibilityLabel="Add a pot"
              onPress={() => {
                router.push("/add-pot/scan");
              }}
              style={({ pressed }) => [
                appStyles.scanButton,
                pressed && appStyles.scanButtonPressed,
              ]}
            >
              <MaterialCommunityIcons
                name="plus"
                size={16}
                color={colors.textPrimary}
              />
              <Text style={appStyles.scanButtonLabel}>Add</Text>
            </Pressable>
          </View>

          <View style={appStyles.potsSectionHeader}>
            <Text style={appStyles.sectionTitle}>My Pots</Text>
          </View>

          {pots.length === 0 ? (
            <View style={styles.emptyState}>
              <MaterialCommunityIcons
                name="leaf-off"
                size={64}
                color={colors.textTertiary}
              />
              <Text style={styles.emptyStateTitle}>No pots yet</Text>
              <Text style={styles.emptyStateText}>
                Tap the Add button to connect your first plant pot
              </Text>
            </View>
          ) : (
            <View style={styles.potsList}>
              {pots.map((pot) => (
                <Pressable
                  key={pot.id}
                  accessibilityRole="button"
                  onPress={() => {
                    router.push({
                      pathname: "/potDetails",
                      params: {
                        potId: pot.id,
                        potName: pot.name,
                      },
                    });
                  }}
                  style={({ pressed }) => [
                    styles.potCard,
                    pressed && styles.potCardPressed,
                  ]}
                >
                  <View style={styles.potCardTopRow}>
                    <View style={styles.potIconContainer}>
                      <MaterialCommunityIcons
                        name="leaf"
                        size={32}
                        color={colors.accent}
                      />
                    </View>
                    <View style={styles.connectionBadge}>
                      <MaterialCommunityIcons
                        name={pot.connected ? "wifi" : "wifi-off"}
                        size={18}
                        color={pot.connected ? colors.success : colors.error}
                      />
                    </View>
                  </View>

                  <View style={styles.potCardName}>
                    <Text style={styles.potNameText}>{pot.name}</Text>
                    <Text style={styles.potLocationText}>
                      {pot.location || "Unknown location"}
                    </Text>
                  </View>

                  {sensorData[pot.id]?.data && (
                    <View style={styles.sensorPreview}>
                      <View style={styles.moistureRow}>
                        <View style={styles.moistureLabel}>
                          <MaterialCommunityIcons
                            name="water-percent"
                            size={14}
                            color={colors.moistureColor}
                          />
                          <Text style={styles.sensorLabel}>Moisture</Text>
                        </View>
                        <View style={styles.moistureIndicator}>
                          {sensorData[pot.id]?.data && (
                            <View
                              style={[
                                styles.moistureBar,
                                {
                                  width: `${Math.min(sensorData[pot.id].data!.soil_moisture, 100)}%`,
                                  backgroundColor: getStatusColor(
                                    getMoistureStatus(
                                      sensorData[pot.id].data!.soil_moisture,
                                    ),
                                  ),
                                },
                              ]}
                            />
                          )}
                        </View>
                        {sensorData[pot.id]?.data && (
                          <Text style={styles.moistureValue}>
                            {Math.round(sensorData[pot.id].data!.soil_moisture)}
                            %
                          </Text>
                        )}
                      </View>
                      <Text style={styles.updateTime}>
                        Updated {formatTime(sensorData[pot.id].lastUpdate || 0)}
                      </Text>
                    </View>
                  )}
                </Pressable>
              ))}
            </View>
          )}
        </ScrollView>
      </View>
    </Background>
  );
}

const styles = StyleSheet.create({
  emptyState: {
    alignItems: "center",
    justifyContent: "center",
    paddingVertical: spacing.xxxl,
    gap: spacing.lg,
  },

  emptyStateTitle: {
    fontSize: fontSizes.xl,
    fontWeight: fontWeights.bold,
    color: colors.textPrimary,
  },

  emptyStateText: {
    fontSize: fontSizes.md,
    color: colors.textSecondary,
    textAlign: "center",
    paddingHorizontal: spacing.lg,
  },

  potsList: {
    flexDirection: "row",
    flexWrap: "wrap",
    justifyContent: "space-between",
    gap: spacing.md,
  },

  potCard: {
    width: "48%",
    backgroundColor: colors.glassBgStrong,
    borderRadius: borderRadius.lg,
    borderWidth: 1,
    borderColor: colors.glassBorderFocus,
    padding: spacing.md,
    gap: spacing.md,
  },

  potCardPressed: {
    opacity: 0.8,
  },

  potCardTopRow: {
    flexDirection: "row",
    justifyContent: "space-between",
    alignItems: "center",
  },

  potIconContainer: {
    width: 48,
    height: 48,
    borderRadius: borderRadius.md,
    backgroundColor: colors.glassBg,
    justifyContent: "center",
    alignItems: "center",
  },

  connectionBadge: {
    width: 32,
    height: 32,
    borderRadius: borderRadius.full,
    backgroundColor: colors.glassBg,
    justifyContent: "center",
    alignItems: "center",
  },

  potCardName: {
    gap: spacing.xs,
  },

  potNameText: {
    fontSize: fontSizes.lg,
    fontWeight: fontWeights.bold,
    color: colors.textPrimary,
  },

  potLocationText: {
    fontSize: fontSizes.sm,
    color: colors.textTertiary,
  },

  sensorPreview: {
    backgroundColor: colors.glassBg,
    borderRadius: borderRadius.md,
    padding: spacing.sm,
    gap: spacing.xs,
    borderTopWidth: 1,
    borderTopColor: colors.glassBorder,
    marginTop: spacing.xs,
  },

  moistureRow: {
    flexDirection: "row",
    alignItems: "center",
    gap: spacing.sm,
  },

  moistureLabel: {
    flexDirection: "row",
    alignItems: "center",
    gap: spacing.xs,
    width: 50,
  },

  sensorLabel: {
    fontSize: fontSizes.xs,
    color: colors.textSecondary,
  },

  moistureIndicator: {
    flex: 1,
    height: 6,
    backgroundColor: colors.glassBgStrong,
    borderRadius: borderRadius.full,
    overflow: "hidden",
  },

  moistureBar: {
    height: "100%",
    borderRadius: borderRadius.full,
  },

  moistureValue: {
    fontSize: fontSizes.xs,
    fontWeight: fontWeights.bold,
    color: colors.textPrimary,
    width: 35,
    textAlign: "right",
  },

  updateTime: {
    fontSize: fontSizes.xs,
    color: colors.textTertiary,
    marginTop: spacing.xs,
  },
});
