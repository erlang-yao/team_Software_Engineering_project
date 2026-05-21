import { describe, expect, it } from 'vitest';
import { api, createTestCredentials } from './test-utils';

describe('Auth API integration', () => {
  it('registers successfully', async () => {
    const credentials = createTestCredentials('auth_register');
    const response = await api.post('/api/auth/register').send(credentials);

    expect(response.status).toBe(200);
    expect(response.body).toMatchObject({ success: true });
    expect(response.body.message).toBeTruthy();
  });

  it('rejects duplicate usernames', async () => {
    const credentials = createTestCredentials('auth_duplicate');

    const firstResponse = await api.post('/api/auth/register').send(credentials);
    const secondResponse = await api.post('/api/auth/register').send(credentials);

    expect(firstResponse.status).toBe(200);
    expect(secondResponse.status).toBe(409);
    expect(secondResponse.body).toMatchObject({ success: false });
    expect(secondResponse.body.error).toBeTruthy();
  });

  it('logs in successfully and returns sessionId', async () => {
    const credentials = createTestCredentials('auth_login');

    await api.post('/api/auth/register').send(credentials);
    const response = await api.post('/api/auth/login').send(credentials);

    expect(response.status).toBe(200);
    expect(response.body).toMatchObject({
      success: true,
      playerName: credentials.username,
    });
    expect(response.body.sessionId).toBeTypeOf('string');
  });

  it('rejects wrong password login', async () => {
    const credentials = createTestCredentials('auth_wrong_password');

    await api.post('/api/auth/register').send(credentials);
    const response = await api.post('/api/auth/login').send({
      username: credentials.username,
      password: `${credentials.password}_wrong`,
    });

    expect(response.status).toBe(401);
    expect(response.body).toMatchObject({ success: false });
    expect(response.body.error).toBeTruthy();
  });
});
