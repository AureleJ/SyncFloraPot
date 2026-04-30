import { StyleSheet } from "react-native";
import { colors } from "./colors";
import { borderRadius, fontSizes, fontWeights, spacing } from "./spacing";

export const globalStyles = StyleSheet.create({
  flex: { flex: 1 },

  glass: {
    backgroundColor: colors.glassBg,
    borderRadius: borderRadius.xl,
    borderWidth: 1,
    borderColor: colors.glassBorder,
    padding: spacing.lg,
  },

  headingLarge: {
    fontSize: fontSizes.xxxl,
    fontWeight: fontWeights.bold,
    color: colors.textPrimary,
    letterSpacing: -0.5,
  },
  headingMedium: {
    fontSize: fontSizes.xl,
    fontWeight: fontWeights.semibold,
    color: colors.textSecondary,
    marginBottom: spacing.lg,
  },
  textPrimary: {
    fontSize: fontSizes.md,
    color: colors.textPrimary,
  },
  textSecondary: {
    fontSize: fontSizes.md,
    color: colors.textSecondary,
  },
  textMuted: {
    fontSize: fontSizes.sm,
    color: colors.textMuted,
  },
  label: {
    fontSize: fontSizes.xs,
    fontWeight: fontWeights.semibold,
    color: colors.textTertiary,
    textTransform: "uppercase",
    letterSpacing: 0.8,
  },

  row: { flexDirection: "row", alignItems: "center" },
  spaceBetween: { justifyContent: "space-between" },
  p: { padding: spacing.lg },
  pVertical: { paddingVertical: spacing.xl },
  pHorizontal: { paddingHorizontal: spacing.lg },

  divider: {
    height: 1,
    backgroundColor: colors.glassBorder,
    marginVertical: spacing.md,
  },
});
