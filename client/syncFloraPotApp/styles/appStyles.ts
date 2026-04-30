import { StyleSheet } from "react-native";
import { colors } from "./colors";
import { borderRadius, fontSizes, fontWeights, spacing } from "./spacing";

export const appStyles = StyleSheet.create({
  mainContainer: {
    flex: 1,
  },

  scrollContainer: {
    paddingHorizontal: spacing.lg,
    paddingTop: spacing.huge,
    paddingBottom: spacing.xl,
  },

  header: {
    flexDirection: "row",
    alignItems: "center",
    justifyContent: "space-between",
    marginBottom: spacing.xxxl,
  },

  heading: {
    fontSize: fontSizes.xxxl,
    fontWeight: fontWeights.bold,
    color: colors.textPrimary,
  },

  subHeading: {
    fontSize: fontSizes.xl,
    fontWeight: fontWeights.semibold,
    color: colors.textSecondary,
    marginBottom: spacing.lg,
  },

  statusBadge: {
    paddingHorizontal: spacing.md,
    paddingVertical: spacing.sm,
    borderRadius: borderRadius.full,
  },

  statusText: {
    color: colors.textPrimary,
    fontWeight: fontWeights.bold,
    fontSize: fontSizes.sm,
  },

  statusConnected: {
    backgroundColor: colors.success,
  },

  statusDisconnected: {
    backgroundColor: colors.error,
  },

  grid: {
    flexDirection: "row",
    flexWrap: "wrap",
    gap: 15,
  },

  emptyState: {
    color: colors.textMuted,
    textAlign: "center",
    marginTop: spacing.huge,
    fontStyle: "italic",
  },
});
