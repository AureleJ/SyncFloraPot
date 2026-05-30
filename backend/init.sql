CREATE EXTENSION IF NOT EXISTS timescaledb;

CREATE TABLE IF NOT EXISTS users (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  email TEXT UNIQUE NOT NULL,
  password_hash TEXT NOT NULL,
  created_at TIMESTAMPTZ DEFAULT now()
);

CREATE TABLE IF NOT EXISTS pots (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  name TEXT NOT NULL,
  location TEXT NOT NULL,
  user_id UUID NOT NULL REFERENCES users(id) ON DELETE CASCADE,
  created_at TIMESTAMPTZ DEFAULT now()
);

CREATE TABLE IF NOT EXISTS devices (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  device_id TEXT UNIQUE NOT NULL,
  secret TEXT NOT NULL,
  pot_id UUID NOT NULL UNIQUE REFERENCES pots(id) ON DELETE CASCADE,
  created_at TIMESTAMPTZ DEFAULT now()
);

CREATE TABLE IF NOT EXISTS metrics (
  time TIMESTAMPTZ NOT NULL,
  pot_id UUID NOT NULL REFERENCES pots(id) ON DELETE CASCADE,
  soil_moisture DOUBLE PRECISION,
  light DOUBLE PRECISION,
  water_level_state TEXT,
  pump_state BOOLEAN
);

SELECT create_hypertable('metrics', 'time', if_not_exists => TRUE);

CREATE INDEX IF NOT EXISTS idx_metrics_pot_time ON metrics (pot_id, time DESC);
CREATE INDEX IF NOT EXISTS idx_pots_user ON pots (user_id);