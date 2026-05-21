import * as fs from 'fs';
import * as path from 'path';
import * as crypto from 'crypto';

function getDataDir(): string {
  return process.env.POKEMON_MUD_DATA_DIR || path.join(__dirname, '..', '..', 'data');
}

function ensureDataDir() {
  const dataDir = getDataDir();
  if (!fs.existsSync(dataDir)) {
    fs.mkdirSync(dataDir, { recursive: true });
  }
}

function hashPassword(password: string): string {
  return crypto.createHash('sha256').update(password).digest('hex');
}

export interface UserRecord {
  username: string;
  passwordHash: string;
  createdAt: string;
}

export interface PlayerSaveData {
  username: string;
  team: any[];
  storage: any[];
  pokeballs: number[];
  items: any[];
  gold: number;
  currentLocation: number;
}

function usersFilePath(): string {
  ensureDataDir();
  return path.join(getDataDir(), 'users.json');
}

function playerSavePath(username: string): string {
  ensureDataDir();
  const safe = username.replace(/[^a-zA-Z0-9\u4e00-\u9fa5_-]/g, '_');
  return path.join(getDataDir(), `save_${safe}.json`);
}

export function loadUsers(): Record<string, UserRecord> {
  const fp = usersFilePath();
  if (!fs.existsSync(fp)) return {};
  try {
    return JSON.parse(fs.readFileSync(fp, 'utf-8'));
  } catch {
    return {};
  }
}

function saveUsers(users: Record<string, UserRecord>) {
  ensureDataDir();
  fs.writeFileSync(usersFilePath(), JSON.stringify(users, null, 2), 'utf-8');
}

export function registerUser(username: string, password: string): UserRecord | null {
  const users = loadUsers();
  if (users[username]) return null;

  const record: UserRecord = {
    username,
    passwordHash: hashPassword(password),
    createdAt: new Date().toISOString(),
  };

  users[username] = record;
  saveUsers(users);
  return record;
}

export function loginUser(username: string, password: string): UserRecord | null {
  const users = loadUsers();
  const record = users[username];
  if (!record) return null;
  if (record.passwordHash !== hashPassword(password)) return null;
  return record;
}

export function loadPlayerSave(username: string): PlayerSaveData | null {
  const fp = playerSavePath(username);
  if (!fs.existsSync(fp)) return null;
  try {
    return JSON.parse(fs.readFileSync(fp, 'utf-8'));
  } catch {
    return null;
  }
}

export function savePlayerData(username: string, data: PlayerSaveData) {
  ensureDataDir();
  fs.writeFileSync(playerSavePath(username), JSON.stringify(data, null, 2), 'utf-8');
}
