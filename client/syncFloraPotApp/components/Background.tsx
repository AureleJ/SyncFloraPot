import React from "react";
import { StyleSheet, View, useWindowDimensions } from "react-native";
import Svg, { Defs, RadialGradient, Rect, Stop } from "react-native-svg";
import { colors } from "../styles/colors";

export default function Background({
  children,
}: {
  children: React.ReactNode;
}) {
  const { width, height } = useWindowDimensions();
  const opacity = 0.3;

  const getRadius = (multiplier: number) => {
    const base = Math.sqrt(width * height);
    return base * multiplier;
  };

  return (
    <View style={styles.container}>
      <Svg
        key={`${width}-${height}`}
        height="100%"
        width="100%"
        style={StyleSheet.absoluteFill}
      >
        <Defs>
          <RadialGradient
            id="cyan"
            cx="0%"
            cy="0%"
            r={getRadius(1.3)}
            fx="0%"
            fy="0%"
            gradientUnits="userSpaceOnUse"
          >
            <Stop
              offset="0%"
              stopColor={colors.gradientCyan}
              stopOpacity={opacity * 0.8}
            />
            <Stop
              offset="100%"
              stopColor={colors.gradientDark}
              stopOpacity="0"
            />
          </RadialGradient>

          <RadialGradient
            id="purple"
            cx="100%"
            cy="100%"
            r={getRadius(1.3)}
            fx="100%"
            fy="100%"
            gradientUnits="userSpaceOnUse"
          >
            <Stop
              offset="0%"
              stopColor={colors.gradientPurple}
              stopOpacity={opacity * 0.7}
            />
            <Stop
              offset="100%"
              stopColor={colors.gradientDark}
              stopOpacity="0"
            />
          </RadialGradient>

          <RadialGradient
            id="emerald"
            cx="50%"
            cy="0%"
            r={getRadius(0.8)}
            fx="50%"
            fy="0%"
            gradientUnits="userSpaceOnUse"
          >
            <Stop
              offset="0%"
              stopColor={colors.gradientEmerald}
              stopOpacity={opacity * 0.5}
            />
            <Stop
              offset="100%"
              stopColor={colors.gradientDark}
              stopOpacity="0"
            />
          </RadialGradient>

          <RadialGradient
            id="gold"
            cx="0%"
            cy="100%"
            r={getRadius(1.2)}
            fx="0%"
            fy="100%"
            gradientUnits="userSpaceOnUse"
          >
            <Stop
              offset="0%"
              stopColor={colors.gradientGold}
              stopOpacity={opacity * 0.7}
            />
            <Stop
              offset="100%"
              stopColor={colors.gradientDark}
              stopOpacity="0"
            />
          </RadialGradient>
        </Defs>

        <Rect x="0" y="0" width="100%" height="100%" fill={colors.background} />

        <Rect x="0" y="0" width="100%" height="100%" fill="url(#cyan)" />
        <Rect x="0" y="0" width="100%" height="100%" fill="url(#purple)" />
        <Rect x="0" y="0" width="100%" height="100%" fill="url(#emerald)" />
        <Rect x="0" y="0" width="100%" height="100%" fill="url(#gold)" />
      </Svg>

      {children}
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: colors.background,
  },
});
