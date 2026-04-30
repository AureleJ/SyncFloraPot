import { useRouter } from "expo-router";
import { StatusBar } from "expo-status-bar";
import React, { useEffect, useState } from "react";
import { ScrollView, Text, TouchableOpacity, View } from "react-native";
import { io, Socket } from "socket.io-client";

import Background from "../components/Background";
import Card from "../components/Card";
import { appStyles } from "../styles/appStyles";

interface RawSensorData {
  soil_moisture: number;
  light: number;
  water_level: number;
  pump_state: boolean;
  timestamp?: number;
}

interface SensorDisplay {
  title: string;
  value: string | number;
}

interface FormattedSensorData {
  soil_moisture: SensorDisplay;
  light: SensorDisplay;
  water_level: SensorDisplay;
  pump_state: SensorDisplay;
}

interface SocketMessage {
  message: string;
}

export default function App() {
  const router = useRouter();

  const [data, setData] = useState<FormattedSensorData | null>(null);
  const [isConnected, setIsConnected] = useState<boolean>(false);

  useEffect(() => {
    const socket: Socket = io("http://192.168.1.26:3000");

    socket.on("connect", () => {
      setIsConnected(true);
    });

    socket.on("syncFloraPotData", (incomingData: SocketMessage) => {
      try {
        const parsedMessage: RawSensorData = JSON.parse(incomingData.message);
        setData(convertData(parsedMessage));
      } catch (error) {
        console.error("Erreur de parsing JSON:", error);
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
          <TouchableOpacity
            style={styles.actionButton}
            onPress={() => router.push("/scan")}
          >
            <Text style={{ color: "white" }}>Scanner mon Pot</Text>
          </TouchableOpacity>

          <View style={appStyles.header}>
            <Text style={appStyles.heading}>SyncFlora</Text>
            <View
              style={[
                appStyles.statusBadge,
                isConnected
                  ? appStyles.statusConnected
                  : appStyles.statusDisconnected,
              ]}
            >
              <Text style={appStyles.statusText}>
                {isConnected ? "Connecté" : "Déconnecté"}
              </Text>
            </View>
          </View>

          <Text style={appStyles.subHeading}>Données des capteurs</Text>

          <View style={appStyles.grid}>
            {data ? (
              Object.entries(data).map(([key, sensor]) => (
                <Card
                  key={key}
                  title={sensor.title}
                  value={sensor.value.toString()}
                />
              ))
            ) : (
              <Text style={appStyles.emptyState}>En attente de données...</Text>
            )}
          </View>
        </ScrollView>
      </View>
    </Background>
  );
}

function convertData(data: RawSensorData): FormattedSensorData {
  return {
    soil_moisture: {
      title: "Humidité du sol",
      value: `${data.soil_moisture}%`,
    },
    light: {
      title: "Intensité lumineuse",
      value: `${data.light}%`,
    },
    water_level: {
      title: "Niveau d'eau",
      value: `${data.water_level}cm`,
    },
    pump_state: {
      title: "État de la pompe",
      value: data.pump_state ? "Activée" : "Désactivée",
    },
  };
}

const styles = {
  actionButton: {
    backgroundColor: "#1a6cf0",
    paddingVertical: 12,
    paddingHorizontal: 20,
    borderRadius: 8,
    alignSelf: "flex-start",
    marginBottom: 20,
  },
};
