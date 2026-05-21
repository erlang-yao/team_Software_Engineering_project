import { describe, expect, it } from 'vitest';
import { api, registerLoginAndStart } from './test-utils';

describe('Team and items API integration', () => {
  it('gets team data successfully', async () => {
    const auth = await registerLoginAndStart('team_view');
    const response = await api.get('/api/team').set('X-Session-Id', auth.sessionId);

    expect(response.status).toBe(200);
    expect(Array.isArray(response.body.team)).toBe(true);
    expect(Array.isArray(response.body.storage)).toBe(true);
    expect(response.body.team.length).toBeGreaterThanOrEqual(1);
  });

  it('gets items successfully and rejects missing session', async () => {
    const auth = await registerLoginAndStart('items_list');

    const successResponse = await api.get('/api/items').set('X-Session-Id', auth.sessionId);
    const unauthorizedResponse = await api.get('/api/items');

    expect(successResponse.status).toBe(200);
    expect(Array.isArray(successResponse.body.items)).toBe(true);
    expect(successResponse.body.items.length).toBeGreaterThan(0);

    expect(unauthorizedResponse.status).toBe(401);
    expect(unauthorizedResponse.body).toMatchObject({ success: false });
    expect(unauthorizedResponse.body.error).toBeTruthy();
  });

  it('uses an item successfully', async () => {
    const auth = await registerLoginAndStart('items_use_success');

    const response = await api
      .post('/api/items/use')
      .set('X-Session-Id', auth.sessionId)
      .send({ itemIndex: 0, pokemonIndex: 0, isTeam: true });

    expect(response.status).toBe(200);
    expect(response.body).toMatchObject({ success: true });
    expect(response.body.message).toBeTruthy();
    expect(Array.isArray(response.body.items)).toBe(true);
  });

  it('returns a failure payload for invalid item parameters', async () => {
    const auth = await registerLoginAndStart('items_use_invalid');

    const response = await api
      .post('/api/items/use')
      .set('X-Session-Id', auth.sessionId)
      .send({ itemIndex: 999, pokemonIndex: 0, isTeam: true });

    expect(response.status).toBe(200);
    expect(response.body).toMatchObject({ success: false });
    expect(response.body.message).toBeTruthy();
  });
});
