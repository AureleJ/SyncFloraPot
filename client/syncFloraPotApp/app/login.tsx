import { MaterialCommunityIcons } from "@expo/vector-icons";
import { useRouter } from "expo-router";
import { StatusBar } from "expo-status-bar";
import React, { useState } from "react";
import {
    Pressable,
    ScrollView,
    StyleSheet,
    Text,
    TextInput,
    View,
} from "react-native";

import Background from "../components/Background";
import { appStyles } from "../styles/appStyles";
import { colors } from "../styles/colors";
import {
    borderRadius,
    fontSizes,
    fontWeights,
    spacing,
} from "../styles/spacing";

export default function Login() {
  const router = useRouter();
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [showPassword, setShowPassword] = useState(false);
  const [isLoading, setIsLoading] = useState(false);

  const handleLogin = async () => {
    setIsLoading(true);
    router.replace("/");
    setIsLoading(false);
  };

  return (
    <Background>
      <View style={appStyles.mainContainer}>
        <StatusBar style="light" translucent backgroundColor="transparent" />

        <ScrollView
          contentContainerStyle={[appStyles.scrollContainer]}
          showsVerticalScrollIndicator={false}
        >
          <View style={styles.logoSection}>
            <View style={styles.logoContainer}>
              <MaterialCommunityIcons
                name="leaf"
                size={64}
                color={colors.primary}
              />
            </View>
            <Text style={styles.appTitle}>SyncFlora</Text>
            <Text style={styles.tagline}>
              Monitor your plants intelligently
            </Text>
          </View>

          <View style={styles.formSection}>
            <View style={styles.inputGroup}>
              <Text style={styles.inputLabel}>Email or Username</Text>
              <View style={styles.inputContainer}>
                <MaterialCommunityIcons
                  name="email-outline"
                  size={20}
                  color={colors.textSecondary}
                  style={styles.inputIcon}
                />
                <TextInput
                  style={styles.input}
                  placeholder="you@example.com"
                  placeholderTextColor={colors.textTertiary}
                  value={email}
                  onChangeText={setEmail}
                  keyboardType="email-address"
                  editable={!isLoading}
                />
              </View>
            </View>

            <View style={styles.inputGroup}>
              <Text style={styles.inputLabel}>Password</Text>
              <View style={styles.inputContainer}>
                <MaterialCommunityIcons
                  name="lock-outline"
                  size={20}
                  color={colors.textSecondary}
                  style={styles.inputIcon}
                />
                <TextInput
                  style={styles.input}
                  placeholder="••••••••"
                  placeholderTextColor={colors.textTertiary}
                  value={password}
                  onChangeText={setPassword}
                  secureTextEntry={!showPassword}
                  editable={!isLoading}
                />
                <Pressable
                  onPress={() => setShowPassword(!showPassword)}
                  style={styles.toggleButton}
                >
                  <MaterialCommunityIcons
                    name={showPassword ? "eye-off" : "eye"}
                    size={20}
                    color={colors.textSecondary}
                  />
                </Pressable>
              </View>
            </View>

            <Pressable style={styles.forgotPasswordLink}>
              <Text style={styles.forgotPasswordText}>Forgot password?</Text>
            </Pressable>

            <Pressable
              onPress={handleLogin}
              disabled={isLoading}
              style={({ pressed }) => [
                styles.loginButton,
                pressed && !isLoading && styles.loginButtonPressed,
                isLoading && styles.loginButtonDisabled,
              ]}
            >
              {isLoading ? (
                <View style={styles.loadingContainer}>
                  <Text style={styles.loginButtonText}>Logging in...</Text>
                </View>
              ) : (
                <View style={styles.buttonContent}>
                  <MaterialCommunityIcons
                    name="login"
                    size={20}
                    color={colors.textPrimary}
                  />
                  <Text style={styles.loginButtonText}>Login</Text>
                </View>
              )}
            </Pressable>
          </View>

          <View style={styles.signupSection}>
            <Text style={styles.signupText}>Don't have an account? </Text>
            <Pressable>
              <Text style={styles.signupLink}>Sign up here</Text>
            </Pressable>
          </View>
        </ScrollView>
      </View>
    </Background>
  );
}

const styles = StyleSheet.create({
  logoSection: {
    alignItems: "center",
    marginBottom: spacing.xxxl,
    gap: spacing.lg,
  },

  logoContainer: {
    width: 100,
    height: 100,
    borderRadius: borderRadius.full,
    backgroundColor: colors.glassBgStrong,
    borderWidth: 1,
    borderColor: colors.glassBorderFocus,
    justifyContent: "center",
    alignItems: "center",
  },

  appTitle: {
    fontSize: fontSizes.xxxl,
    fontWeight: fontWeights.bold,
    color: colors.textPrimary,
  },

  tagline: {
    fontSize: fontSizes.md,
    color: colors.textSecondary,
    textAlign: "center",
  },

  formSection: {
    gap: spacing.lg,
    marginBottom: spacing.xxxl,
  },

  inputGroup: {
    gap: spacing.sm,
  },

  inputLabel: {
    fontSize: fontSizes.sm,
    fontWeight: fontWeights.bold,
    color: colors.textPrimary,
    paddingHorizontal: spacing.sm,
  },

  inputContainer: {
    flexDirection: "row",
    alignItems: "center",
    backgroundColor: colors.glassBgStrong,
    borderRadius: borderRadius.lg,
    borderWidth: 1,
    borderColor: colors.glassBorderFocus,
    paddingHorizontal: spacing.md,
    height: 48,
    gap: spacing.sm,
  },

  inputIcon: {
    marginRight: spacing.xs,
  },

  input: {
    flex: 1,
    fontSize: fontSizes.md,
    color: colors.textPrimary,
    fontWeight: "500",
  },

  toggleButton: {
    padding: spacing.xs,
  },

  forgotPasswordLink: {
    alignSelf: "flex-end",
    paddingHorizontal: spacing.sm,
  },

  forgotPasswordText: {
    fontSize: fontSizes.sm,
    color: colors.primary,
    fontWeight: "500",
  },

  loginButton: {
    backgroundColor: colors.primary,
    borderRadius: borderRadius.lg,
    paddingVertical: spacing.lg,
    marginTop: spacing.md,
    justifyContent: "center",
    alignItems: "center",
  },

  loginButtonPressed: {
    opacity: 0.85,
  },

  loginButtonDisabled: {
    opacity: 0.6,
  },

  buttonContent: {
    flexDirection: "row",
    alignItems: "center",
    gap: spacing.md,
  },

  loadingContainer: {
    flexDirection: "row",
    alignItems: "center",
  },

  loginButtonText: {
    fontSize: fontSizes.lg,
    fontWeight: fontWeights.bold,
    color: colors.textPrimary,
  },

  divider: {
    flexDirection: "row",
    alignItems: "center",
    marginVertical: spacing.lg,
    gap: spacing.md,
  },

  dividerLine: {
    flex: 1,
    height: 1,
    backgroundColor: colors.glassBorder,
  },

  dividerText: {
    fontSize: fontSizes.sm,
    color: colors.textTertiary,
  },

  socialSection: {
    flexDirection: "row",
    justifyContent: "center",
    gap: spacing.lg,
  },

  socialButton: {
    width: 56,
    height: 56,
    borderRadius: borderRadius.lg,
    backgroundColor: colors.glassBgStrong,
    borderWidth: 1,
    borderColor: colors.glassBorderFocus,
    justifyContent: "center",
    alignItems: "center",
  },

  socialButtonPressed: {
    opacity: 0.8,
  },

  signupSection: {
    flexDirection: "row",
    justifyContent: "center",
    alignItems: "center",
    paddingTop: spacing.lg,
  },

  signupText: {
    fontSize: fontSizes.sm,
    color: colors.textSecondary,
  },

  signupLink: {
    fontSize: fontSizes.sm,
    fontWeight: fontWeights.bold,
    color: colors.primary,
  },
});
