import request, { type SuperTest, type Test } from 'supertest';
import app from '../../src/app';

export const api: SuperTest<Test> = request(app);

export function createTestCredentials(prefix: string) {
  const trimmedPrefix = prefix.slice(0, 8);
  const uniqueSuffix = `${Date.now().toString(36).slice(-6)}${Math.random().toString(36).slice(2, 6)}`;
  return {
    username: `${trimmedPrefix}_${uniqueSuffix}`.slice(0, 20),
    password: `pw_${uniqueSuffix}`,
  };
}

export async function registerAndLogin(prefix: string) {
  const credentials = createTestCredentials(prefix);

  const registerResponse = await api.post('/api/auth/register').send(credentials);
  const loginResponse = await api.post('/api/auth/login').send(credentials);

  return {
    ...credentials,
    registerResponse,
    loginResponse,
    sessionId: loginResponse.body.sessionId as string,
  };
}

export async function registerLoginAndStart(prefix: string, starterIndex = 1) {
  const auth = await registerAndLogin(prefix);
  const startResponse = await api
    .post('/api/game/start')
    .set('X-Session-Id', auth.sessionId)
    .send({ starterIndex });

  return {
    ...auth,
    startResponse,
  };
}
