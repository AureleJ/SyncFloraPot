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

  topBar: {
    flexDirection: "row",
    alignItems: "flex-start",
    justifyContent: "space-between",
    marginBottom: spacing.lg,
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
    marginBottom: spacing.xs,
  },

  subHeading: {
    fontSize: fontSizes.lg,
    fontWeight: fontWeights.semibold,
    color: colors.textSecondary,
    marginBottom: 0,
  },

  scanButton: {
    minHeight: 36,
    borderRadius: borderRadius.full,
    backgroundColor: colors.primary,
    justifyContent: "center",
    alignItems: "center",
    flexDirection: "row",
    gap: spacing.xs,
    paddingHorizontal: spacing.md,
    paddingVertical: spacing.xs,
    borderWidth: 1,
    borderColor: colors.glassBorderFocus,
  },

  scanButtonPressed: {
    opacity: 0.88,
  },

  scanButtonLabel: {
    color: colors.textPrimary,
    fontSize: fontSizes.sm,
    fontWeight: fontWeights.semibold,
  },

  potsSectionHeader: {
    marginBottom: spacing.md,
  },

  sectionTitle: {
    fontSize: fontSizes.xl,
    color: colors.textPrimary,
    fontWeight: fontWeights.bold,
  },

  sectionSubtitle: {
    fontSize: fontSizes.sm,
    color: colors.textSecondary,
    marginTop: spacing.xs,
  },

  potsList: {
    flexDirection: "row",
    flexWrap: "wrap",
    justifyContent: "space-between",
    gap: spacing.md,
  },

  statusBadge: {
    flexDirection: "row",
    alignItems: "center",
    gap: spacing.xs,
    backgroundColor: colors.successBg,
    borderRadius: borderRadius.full,
    paddingHorizontal: spacing.md,
    paddingVertical: spacing.sm,
  },

  potCard: {
    width: "48%",
    minHeight: 130,
    backgroundColor: colors.glassBgStrong,
    borderRadius: borderRadius.lg,
    borderWidth: 1,
    borderColor: colors.glassBorderFocus,
    padding: spacing.md,
    justifyContent: "space-between",
  },

  potCardTopRow: {
    flexDirection: "row",
    justifyContent: "space-between",
    alignItems: "center",
    marginBottom: spacing.lg,
  },

  potIconLarge: {
    width: 44,
    height: 44,
    justifyContent: "center",
    alignItems: "center",
  },

  wifiStatus: {
    width: 32,
    height: 32,
    justifyContent: "center",
    alignItems: "center",
  },

  potCardBottomRow: {
    gap: 2,
  },

  potCardName: {
    fontSize: fontSizes.lg,
    fontWeight: fontWeights.bold,
    color: colors.textPrimary,
    lineHeight: 22,
  },

  potCardLocation: {
    fontSize: fontSizes.xs,
    color: colors.textTertiary,
    lineHeight: 14,
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
    gap: spacing.md,
  },

  emptyState: {
    color: colors.textMuted,
    textAlign: "center",
    marginTop: spacing.huge,
    fontStyle: "italic",
  },
});
