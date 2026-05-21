import express from 'express';
import path from 'path';
import cors from 'cors';
import gameRoutes from './routes/game-routes';
import playerRoutes from './routes/player-routes';
import mapRoutes from './routes/map-routes';
import battleRoutes from './routes/battle-routes';
import teamRoutes from './routes/team-routes';
import itemRoutes from './routes/item-routes';
import authRoutes from './routes/auth-routes';
import shopRoutes from './routes/shop-routes';
import { errorHandler } from './middleware/error-handler';

const app = express();

app.use(cors());
app.use(express.json());

app.use('/api/game', gameRoutes);
app.use('/api/player', playerRoutes);
app.use('/api/map', mapRoutes);
app.use('/api/battle', battleRoutes);
app.use('/api/team', teamRoutes);
app.use('/api/items', itemRoutes);
app.use('/api/auth', authRoutes);
app.use('/api/shop', shopRoutes);

app.get('/api/health', (_req, res) => {
  res.json({ status: 'ok', timestamp: Date.now() });
});

const publicDir = path.join(__dirname, '..', 'public');
app.use(express.static(publicDir));

app.get('*', (_req, res) => {
  res.sendFile(path.join(publicDir, 'index.html'));
});

app.use(errorHandler);

export default app;
