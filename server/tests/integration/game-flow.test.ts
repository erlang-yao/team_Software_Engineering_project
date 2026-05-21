import { describe, expect, it } from 'vitest';
import { api, registerAndLogin } from './test-utils';

describe('Game flow API integration', () => {
  it('starts the game, reads player status, and moves with X-Session-Id', async () => {
    const auth = await registerAndLogin('game_flow');

    expect(auth.registerResponse.status).toBe(200);
    expect(auth.loginResponse.status).toBe(200);
    expect(auth.sessionId).toBeTypeOf('string');

    const startResponse = await api
      .post('/api/game/start')
      .set('X-Session-Id', auth.sessionId)
      .send({ starterIndex: 1 });

    expect(startResponse.status).toBe(200);
    expect(startResponse.body.sessionId).toBe(auth.sessionId);
    expect(startResponse.body.teamCount).toBeGreaterThanOrEqual(1);

    const statusResponse = await api
      .get('/api/player/status')
      .set('X-Session-Id', auth.sessionId);

    expect(statusResponse.status).toBe(200);
    expect(statusResponse.body).toMatchObject({
      playerName: auth.username,
    });
    expect(statusResponse.body.currentLocation).toBeTruthy();

    const moveResponse = await api
      .post('/api/game/move')
      .set('X-Session-Id', auth.sessionId)
      .send({ direction: 'e' });

    expect(moveResponse.status).toBe(200);
    expect(moveResponse.body).toMatchObject({ success: true });
    expect(moveResponse.body.newLocation).toBeTruthy();
  });

  it('rejects invalid direction', async () => {
    const auth = await registerAndLogin('game_flow_invalid_direction');

    await api
      .post('/api/game/start')
      .set('X-Session-Id', auth.sessionId)
      .send({ starterIndex: 1 });

    const response = await api
      .post('/api/game/move')
      .set('X-Session-Id', auth.sessionId)
      .send({ direction: 'invalid-direction' });

    expect(response.status).toBe(400);
    expect(response.body).toMatchObject({ success: false });
    expect(response.body.message).toBeTruthy();
  });

  it('rejects protected endpoints without X-Session-Id', async () => {
    const response = await api.get('/api/player/status');

    expect(response.status).toBe(401);
    expect(response.body).toMatchObject({ success: false });
    expect(response.body.error).toBeTruthy();
  });
});
