import fs from 'fs';
import path from 'path';

const testDataDir = path.join(__dirname, '.tmp', 'integration-data');

fs.rmSync(testDataDir, { recursive: true, force: true });
fs.mkdirSync(testDataDir, { recursive: true });

process.env.POKEMON_MUD_DATA_DIR = testDataDir;

process.on('exit', () => {
  fs.rmSync(testDataDir, { recursive: true, force: true });
});
