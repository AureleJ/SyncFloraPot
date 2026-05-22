import { useLocalSearchParams, useRouter } from "expo-router";
import React, { useMemo, useState } from "react";
import {
    KeyboardAvoidingView,
    Platform,
    Pressable,
    StyleSheet,
    Text,
    TextInput,
    View,
} from "react-native";

import Background from "../../components/Background";
import { usePots } from "../../context/PotsContext";
import { colors } from "../../styles/colors";
import {
    borderRadius,
    fontSizes,
    fontWeights,
    spacing,
} from "../../styles/spacing";

export default function AddPotDetailsPage() {
  const { qrData } = useLocalSearchParams<{ qrData?: string }>();
  const router = useRouter();
  const { addPot } = usePots();

  const [name, setName] = useState("");
  const [location, setLocation] = useState("");

  const canSubmit = useMemo(() => {
    return name.trim().length > 1 && location.trim().length > 1;
  }, [name, location]);

  return (
    <Background>
      <KeyboardAvoidingView
        style={styles.container}
        behavior={Platform.OS === "ios" ? "padding" : undefined}
      >
        <Text style={styles.title}>Pot Info</Text>
        <Text style={styles.subtitle}>
          Give your pot a name and indicate where it is placed.
        </Text>

        <View style={styles.inputGroup}>
          <Text style={styles.label}>Pot Name</Text>
          <TextInput
            value={name}
            onChangeText={setName}
            placeholder="Ex: Basil Pot"
            placeholderTextColor={colors.textTertiary}
            style={styles.input}
          />
        </View>

        <View style={styles.inputGroup}>
          <Text style={styles.label}>Location</Text>
          <TextInput
            value={location}
            onChangeText={setLocation}
            placeholder="Ex: Kitchen"
            placeholderTextColor={colors.textTertiary}
            style={styles.input}
          />
        </View>

        <Pressable
          disabled={!canSubmit}
          onPress={() => {
            const newPot = addPot({
              name,
              location,
              qrData: qrData ? String(qrData) : undefined,
            });

            router.replace({
              pathname: "/",
              params: { newPotId: newPot.id },
            });
          }}
          style={({ pressed }) => [
            styles.submitButton,
            !canSubmit && styles.submitButtonDisabled,
            canSubmit && pressed && styles.submitButtonPressed,
          ]}
        >
          <Text style={styles.submitButtonText}>Add Pot</Text>
        </Pressable>
      </KeyboardAvoidingView>
    </Background>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    paddingHorizontal: spacing.lg,
    paddingTop: spacing.huge,
    paddingBottom: spacing.xl,
    gap: spacing.lg,
  },
  title: {
    fontSize: fontSizes.xxl,
    color: colors.textPrimary,
    fontWeight: fontWeights.bold,
  },
  subtitle: {
    color: colors.textSecondary,
    fontSize: fontSizes.md,
  },
  inputGroup: {
    gap: spacing.xs,
  },
  label: {
    color: colors.textSecondary,
    fontSize: fontSizes.sm,
    fontWeight: fontWeights.medium,
  },
  input: {
    height: 48,
    borderRadius: borderRadius.lg,
    borderWidth: 1,
    borderColor: colors.glassBorder,
    backgroundColor: colors.glassBg,
    color: colors.textPrimary,
    paddingHorizontal: spacing.md,
    fontSize: fontSizes.md,
  },
  submitButton: {
    minHeight: 50,
    borderRadius: borderRadius.lg,
    backgroundColor: colors.primary,
    borderWidth: 1,
    borderColor: colors.glassBorderFocus,
    justifyContent: "center",
    alignItems: "center",
    marginTop: spacing.md,
  },
  submitButtonDisabled: {
    opacity: 0.45,
  },
  submitButtonPressed: {
    opacity: 0.9,
  },
  submitButtonText: {
    color: colors.textPrimary,
    fontSize: fontSizes.md,
    fontWeight: fontWeights.semibold,
  },
});
