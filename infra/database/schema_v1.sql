-- Vietnam Online Database Schema v1
-- Theo doc 17.7 + 35.5 (PostgreSQL / Supabase)

CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- Auth & Users
CREATE TABLE IF NOT EXISTS users (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    email VARCHAR(255) UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    status VARCHAR(20) DEFAULT 'active',
    created_at TIMESTAMPTZ DEFAULT NOW(),
    last_login TIMESTAMPTZ
);

CREATE TABLE IF NOT EXISTS sessions (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id UUID REFERENCES users(id) ON DELETE CASCADE,
    refresh_token TEXT,
    device_info TEXT,
    expires_at TIMESTAMPTZ,
    created_at TIMESTAMPTZ DEFAULT NOW()
);

-- Characters (FR-02, FR-10)
CREATE TABLE IF NOT EXISTS characters (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id UUID REFERENCES users(id) ON DELETE CASCADE,
    display_name VARCHAR(32) NOT NULL,
    zone_id VARCHAR(8) DEFAULT 'HOI_AN',
    position_json JSONB DEFAULT '{"x":0,"y":0,"z":0}',
    appearance_json JSONB DEFAULT '{}',
    stats_json JSONB DEFAULT '{}',
    wallet_vnd BIGINT DEFAULT 0,
    explorer_lv SMALLINT DEFAULT 1,
    cultural_knowledge SMALLINT DEFAULT 0,
    reputation_score REAL DEFAULT 3.0,
    updated_at TIMESTAMPTZ DEFAULT NOW()
);

-- World / POI
CREATE TABLE IF NOT EXISTS locations (
    id VARCHAR(32) PRIMARY KEY,
    name_vi VARCHAR(200) NOT NULL,
    name_en VARCHAR(200),
    lat DOUBLE PRECISION,
    lng DOUBLE PRECISION,
    zone_id VARCHAR(8),
    tier SMALLINT,
    category VARCHAR(32),
    description TEXT,
    opening_hours JSONB,
    entry_fee_vnd BIGINT DEFAULT 0
);

CREATE TABLE IF NOT EXISTS visits (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    character_id UUID REFERENCES characters(id) ON DELETE CASCADE,
    location_id VARCHAR(32) REFERENCES locations(id),
    visited_at TIMESTAMPTZ DEFAULT NOW(),
    photo_count INT DEFAULT 0,
    UNIQUE(character_id, location_id)
);

-- Gameplay
CREATE TABLE IF NOT EXISTS quests (
    id VARCHAR(64) PRIMARY KEY,
    character_id UUID REFERENCES characters(id) ON DELETE CASCADE,
    quest_type VARCHAR(20),
    status VARCHAR(20),
    progress_json JSONB DEFAULT '{}',
    started_at TIMESTAMPTZ,
    completed_at TIMESTAMPTZ
);

-- Economy (G.1)
CREATE TABLE IF NOT EXISTS transactions (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    character_id UUID REFERENCES characters(id),
    tx_type VARCHAR(32),
    amount BIGINT,
    currency VARCHAR(10) DEFAULT 'VND',
    item_ref TEXT,
    created_at TIMESTAMPTZ DEFAULT NOW()
);

-- Social / Chat (FR-05, expiry 24h)
CREATE TABLE IF NOT EXISTS messages (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    channel VARCHAR(20),
    sender_id UUID,
    content TEXT,
    created_at TIMESTAMPTZ DEFAULT NOW(),
    expires_at TIMESTAMPTZ DEFAULT NOW() + INTERVAL '24 hours'
);

CREATE INDEX IF NOT EXISTS idx_messages_expires ON messages(expires_at);
CREATE INDEX IF NOT EXISTS idx_characters_user ON characters(user_id);
CREATE INDEX IF NOT EXISTS idx_visits_character ON visits(character_id);
