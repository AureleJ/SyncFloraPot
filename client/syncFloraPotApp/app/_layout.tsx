import { Stack } from "expo-router";
import { PotsProvider } from "../context/PotsContext";

export default function Layout() {
  return (
    <PotsProvider>
      <Stack screenOptions={{ headerShown: false }}>
        <Stack.Screen name="login" />
        <Stack.Screen name="index" />
        <Stack.Screen
          name="scan"
          options={{
            presentation: "modal",
            animation: "slide_from_bottom",
          }}
        />
        <Stack.Screen
          name="add-pot/scan"
          options={{
            presentation: "modal",
            animation: "slide_from_bottom",
          }}
        />
        <Stack.Screen
          name="add-pot/details"
          options={{
            presentation: "modal",
            animation: "slide_from_right",
          }}
        />
      </Stack>
    </PotsProvider>
  );
}
