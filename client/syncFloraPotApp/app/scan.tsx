import { useRouter } from "expo-router";
import React from "react";
import { StyleSheet, Text, TouchableOpacity, View } from "react-native";
import Background from "../components/Background";
import QRScanner from "../components/QRScanner";

export default function ScanPage() {
  const router = useRouter();

  const handleScan = (data: string) => {
    console.log("Données récupérées :", data);
    router.back();
  };

  return (
    <Background>
      <View style={styles.container}>
        <QRScanner onScan={handleScan} />

        <TouchableOpacity
          style={styles.backButton}
          onPress={() => router.back()}
        >
          <Text style={styles.backText}>Annuler</Text>
        </TouchableOpacity>
      </View>
    </Background>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1 },
  backButton: {
    position: "absolute",
    top: 50,
    left: 20,
    backgroundColor: "rgba(0,0,0,0.6)",
    padding: 10,
    borderRadius: 8,
  },
  backText: { color: "white", fontWeight: "bold" },
});
