import React, { useEffect, useState } from 'react';
import { useGameState } from './store/game-context';
import LoginScreen from './components/screens/LoginScreen';
import StartScreen from './components/screens/StartScreen';
import GameScreen from './components/screens/GameScreen';
import BattleScreen from './components/screens/BattleScreen';
import { api, setSessionId, clearSessionId } from './api/client';
import type { PlayerStatusResponse } from './types';

const App: React.FC = () => {
  const { state, dispatch } = useGameState();
  const [needsStarter, setNeedsStarter] = useState(false);
  const [checkingSession, setCheckingSession] = useState(false);

  useEffect(() => {
    if (state.notification) {
      const timer = setTimeout(() => dispatch({ type: 'SET_NOTIFICATION', notification: null }), 3000);
      return () => clearTimeout(timer);
    }
  }, [state.notification, dispatch]);

  useEffect(() => {
    if (state.error) {
      const timer = setTimeout(() => dispatch({ type: 'SET_ERROR', error: null }), 5000);
      return () => clearTimeout(timer);
    }
  }, [state.error, dispatch]);

  // 验证并恢复已保存的 session
  useEffect(() => {
    const savedSessionId = localStorage.getItem('pokemon_session_id');
    if (!savedSessionId) return;

    setCheckingSession(true);
    setSessionId(savedSessionId);

    api.get<PlayerStatusResponse>('/player/status')
      .then((status) => {
        dispatch({ type: 'SET_SESSION', sessionId: savedSessionId });
        dispatch({ type: 'UPDATE_PLAYER', status });
        dispatch({ type: 'SET_SCREEN', screen: 'game' });
      })
      .catch(() => {
        clearSessionId();
      })
      .finally(() => {
        setCheckingSession(false);
      });
  }, [dispatch]);

  const handleLogin = (needsStarter: boolean) => {
    if (needsStarter) {
      setNeedsStarter(true);
      dispatch({ type: 'SET_SCREEN', screen: 'start' });
    } else {
      dispatch({ type: 'SET_SCREEN', screen: 'game' });
    }
  };

  if (checkingSession) {
    return (
      <div style={{
        display: 'flex', alignItems: 'center', justifyContent: 'center',
        width: '100vw', height: '100vh', background: '#0a0a1a',
      }}>
        <p style={{ color: '#FFD700', fontSize: 18 }}>正在恢复游戏...</p>
      </div>
    );
  }

  return (
    <div style={{ width: '100vw', minHeight: '100vh', background: '#0a0a1a' }}>
      {!state.sessionId && <LoginScreen onLogin={handleLogin} />}
      {state.screen === 'start' && needsStarter && <StartScreen skipToGame={() => dispatch({ type: 'SET_SCREEN', screen: 'game' })} />}
      {(state.screen === 'game' || state.screen === 'battle') && <GameScreen />}
      {state.screen === 'battle' && <BattleScreen />}

      {state.loading && (
        <div style={{
          position: 'fixed', top: 0, left: 0, right: 0,
          height: 3, background: '#FFD700', zIndex: 2000,
          animation: 'loadingBar 1s infinite',
        }} />
      )}

      {state.error && (
        <div style={{
          position: 'fixed', bottom: 20, left: '50%', transform: 'translateX(-50%)',
          background: 'rgba(244,67,54,0.95)', color: '#FFF',
          padding: '12px 24px', borderRadius: 10, fontSize: 14,
          zIndex: 2000, maxWidth: 500, textAlign: 'center',
        }}>
          {state.error}
        </div>
      )}
    </div>
  );
};

export default App;
