import React from "react";
import { StyleSheet, Text, View } from "react-native";
import { colors } from "../styles/colors";
import {
  borderRadius,
  fontSizes,
  fontWeights,
  spacing,
} from "../styles/spacing";

interface CardProps {
  title: string;
  value: string;
  icon?: string;
}

export default function Card({ title, value, icon }: CardProps) {
  return (
    <View style={styles.container}>
      <View style={styles.topRow}>
        <Text style={styles.title}>{title}</Text>
        {icon ? <Text style={styles.icon}>{icon}</Text> : null}
      </View>

      <Text style={styles.value}>{value}</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    width: "50%",
    maxWidth: 200,
    backgroundColor: colors.glassBg,
    borderRadius: borderRadius.xl,
    borderWidth: 1,
    borderColor: colors.glassBorder,
    padding: spacing.lg,
    overflow: "hidden",
    position: "relative",
  },

  topRow: {
    flexDirection: "row",
    justifyContent: "space-between",
    alignItems: "center",
    marginBottom: spacing.sm,
  },

  title: {
    fontSize: fontSizes.xs,
    fontWeight: fontWeights.semibold,
    color: colors.textTertiary,
    textTransform: "uppercase",
    letterSpacing: 0.7,
    flex: 1,
    marginRight: spacing.xs,
  },

  icon: {
    fontSize: fontSizes.lg,
  },

  value: {
    fontSize: fontSizes.xxl,
    fontWeight: fontWeights.bold,
    letterSpacing: -0.5,
    color: colors.textPrimary,
    marginBottom: spacing.xs,
  },
});
