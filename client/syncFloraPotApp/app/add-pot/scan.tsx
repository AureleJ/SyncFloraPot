import { MaterialCommunityIcons } from "@expo/vector-icons";
import { useRouter } from "expo-router";
import React, { useState } from "react";
import { Pressable, StyleSheet, Text, View } from "react-native";

import Background from "../../components/Background";
import QRScanner from "../../components/QRScanner";
import { colors } from "../../styles/colors";
import {
    borderRadius,
    fontSizes,
    fontWeights,
    spacing,
} from "../../styles/spacing";

export default function AddPotScanPage() {
  const router = useRouter();
  const [qrData, setQrData] = useState<string | null>(null);

  const handleScan = (data: string) => {
    setQrData(data);
  };

  return (
    <Background>
      <View style={styles.container}>
        <View style={styles.topBar}>
          <Text style={styles.title}>Add a Pot</Text>
          <Pressable onPress={() => router.back()} style={styles.cancelButton}>
            <Text style={styles.cancelText}>Cancel</Text>
          </Pressable>
        </View>

        <Text style={styles.subtitle}>
          Scan the QR code to pair the pot to Wi-Fi.
        </Text>

        <View style={styles.scannerFrame}>
          <QRScanner onScan={handleScan} showAlert={false} />
        </View>

        <View style={styles.statusRow}>
          <MaterialCommunityIcons
            name={qrData ? "wifi-check" : "qrcode-scan"}
            size={18}
            color={qrData ? colors.success : colors.textSecondary}
          />
          <Text style={styles.statusText}>
            {qrData
              ? "QR detected, provisioning complete."
              : "Waiting for QR scan..."}
          </Text>
        </View>

        <Pressable
          disabled={!qrData}
          onPress={() => {
            router.push({
              pathname: "/add-pot/details",
              params: { qrData },
            });
          }}
          style={({ pressed }) => [
            styles.nextButton,
            !qrData && styles.nextButtonDisabled,
            pressed && qrData && styles.nextButtonPressed,
          ]}
        >
          <Text style={styles.nextButtonText}>Continue</Text>
        </Pressable>
      </View>
    </Background>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    paddingHorizontal: spacing.lg,
    paddingTop: spacing.huge,
    paddingBottom: spacing.xl,
    gap: spacing.md,
  },
  topBar: {
    flexDirection: "row",
    alignItems: "center",
    justifyContent: "space-between",
  },
  title: {
    fontSize: fontSizes.xxl,
    color: colors.textPrimary,
    fontWeight: fontWeights.bold,
  },
  cancelButton: {
    paddingHorizontal: spacing.md,
    paddingVertical: spacing.sm,
    borderRadius: borderRadius.md,
    borderWidth: 1,
    borderColor: colors.glassBorder,
    backgroundColor: colors.glassBg,
  },
  cancelText: {
    color: colors.textPrimary,
    fontWeight: fontWeights.medium,
  },
  subtitle: {
    color: colors.textSecondary,
    fontSize: fontSizes.md,
  },
  scannerFrame: {
    flex: 1,
    overflow: "hidden",
    borderRadius: borderRadius.xl,
    borderWidth: 1,
    borderColor: colors.glassBorder,
    backgroundColor: colors.backgroundSecondary,
  },
  statusRow: {
    flexDirection: "row",
    alignItems: "center",
    gap: spacing.sm,
  },
  statusText: {
    color: colors.textSecondary,
    fontSize: fontSizes.sm,
  },
  nextButton: {
    minHeight: 48,
    borderRadius: borderRadius.lg,
    backgroundColor: colors.primary,
    borderWidth: 1,
    borderColor: colors.glassBorderFocus,
    justifyContent: "center",
    alignItems: "center",
  },
  nextButtonDisabled: {
    opacity: 0.45,
  },
  nextButtonPressed: {
    opacity: 0.9,
  },
  nextButtonText: {
    color: colors.textPrimary,
    fontSize: fontSizes.md,
    fontWeight: fontWeights.semibold,
  },
});
