import React, { createContext, useContext, useMemo, useState } from "react";

export interface Pot {
  id: string;
  name: string;
  connected: boolean;
  location?: string;
  qrData?: string;
}

interface AddPotInput {
  name: string;
  location: string;
  qrData?: string;
}

interface PotsContextValue {
  pots: Pot[];
  addPot: (input: AddPotInput) => Pot;
}

const PotsContext = createContext<PotsContextValue | undefined>(undefined);

const initialPots: Pot[] = [
  { id: "1", name: "Pot 1", connected: true, location: "Salon" },
  { id: "2", name: "Pot 2", connected: true, location: "Cuisine" },
  { id: "3", name: "Pot 3", connected: false, location: "Bureau" },
];

export function PotsProvider({ children }: { children: React.ReactNode }) {
  const [pots, setPots] = useState<Pot[]>(initialPots);

  const addPot = (input: AddPotInput) => {
    const newPot: Pot = {
      id: Date.now().toString(),
      name: input.name.trim(),
      location: input.location.trim(),
      qrData: input.qrData,
      connected: true,
    };

    setPots((previous) => [newPot, ...previous]);
    return newPot;
  };

  const value = useMemo(() => ({ pots, addPot }), [pots]);

  return <PotsContext.Provider value={value}>{children}</PotsContext.Provider>;
}

export function usePots() {
  const context = useContext(PotsContext);

  if (!context) {
    throw new Error("usePots must be used inside PotsProvider");
  }

  return context;
}
