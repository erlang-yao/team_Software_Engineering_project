import { describe, expect, it } from 'vitest';
import { api } from './test-utils';

describe('GET /api/health', () => {
  it('returns 200 with status ok', async () => {
    const response = await api.get('/api/health');

    expect(response.status).toBe(200);
    expect(response.body).toMatchObject({
      status: 'ok',
    });
    expect(response.body.timestamp).toBeTypeOf('number');
  });
});
