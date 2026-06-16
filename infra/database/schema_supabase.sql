-- Vietnam Online — Supabase schema (chay trong SQL Editor)
-- Dashboard: https://supabase.com/dashboard/project/_/sql
-- Dung auth.users co san cua Supabase Auth (FR-01)

CREATE TABLE IF NOT EXISTS public.characters (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    user_id UUID REFERENCES auth.users(id) ON DELETE CASCADE NOT NULL UNIQUE,
    display_name VARCHAR(32) NOT NULL DEFAULT 'Du khach',
    zone_id VARCHAR(8) DEFAULT 'HOI_AN',
    position_json JSONB DEFAULT '{"x":0,"y":0,"z":0}',
    appearance_json JSONB DEFAULT '{}',
    stats_json JSONB DEFAULT '{}',
    wallet_vnd BIGINT DEFAULT 0,
    explorer_lv SMALLINT DEFAULT 1,
    cultural_knowledge SMALLINT DEFAULT 0,
    reputation_score REAL DEFAULT 3.0,
    quests_json JSONB DEFAULT '[]',
    photo_count SMALLINT DEFAULT 0,
    visited_pois TEXT[] DEFAULT '{}',
    updated_at TIMESTAMPTZ DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_characters_user_id ON public.characters(user_id);
CREATE INDEX IF NOT EXISTS idx_characters_zone ON public.characters(zone_id);

ALTER TABLE public.characters ENABLE ROW LEVEL SECURITY;

DROP POLICY IF EXISTS "characters_select_own" ON public.characters;
CREATE POLICY "characters_select_own" ON public.characters
    FOR SELECT USING (auth.uid() = user_id);

DROP POLICY IF EXISTS "characters_insert_own" ON public.characters;
CREATE POLICY "characters_insert_own" ON public.characters
    FOR INSERT WITH CHECK (auth.uid() = user_id);

DROP POLICY IF EXISTS "characters_update_own" ON public.characters;
CREATE POLICY "characters_update_own" ON public.characters
    FOR UPDATE USING (auth.uid() = user_id);

-- Visits / quests (optional Phase 1+)
CREATE TABLE IF NOT EXISTS public.visits (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    character_id UUID REFERENCES public.characters(id) ON DELETE CASCADE,
    location_id VARCHAR(32) NOT NULL,
    visited_at TIMESTAMPTZ DEFAULT NOW(),
    UNIQUE(character_id, location_id)
);

ALTER TABLE public.visits ENABLE ROW LEVEL SECURITY;

DROP POLICY IF EXISTS "visits_own" ON public.visits;
CREATE POLICY "visits_own" ON public.visits
    FOR ALL USING (
        character_id IN (SELECT id FROM public.characters WHERE user_id = auth.uid())
    );
