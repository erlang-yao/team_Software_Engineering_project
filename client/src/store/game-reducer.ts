import { GameState, GameAction } from '../types';

export const initialState: GameState = {
  sessionId: null,
  screen: 'start',
  playerName: '',
  playerStatus: null,
  map: null,
  battle: null,
  team: [],
  storage: [],
  items: [],
  loading: false,
  error: null,
  notification: null,
};

export function gameReducer(state: GameState, action: GameAction): GameState {
  switch (action.type) {
    case 'SET_SESSION':
      return { ...state, sessionId: action.sessionId, error: null };

    case 'SET_SCREEN':
      return { ...state, screen: action.screen };

    case 'UPDATE_PLAYER':
      return {
        ...state,
        playerStatus: action.status,
        playerName: action.status.playerName,
      };

    case 'UPDATE_MAP':
      return { ...state, map: action.map };

    case 'ENTER_BATTLE':
      return { ...state, screen: 'battle', battle: action.battle };

    case 'UPDATE_BATTLE':
      if (!state.battle) return state;
      return {
        ...state,
        battle: {
          ...state.battle,
          playerPokeHp: action.battle.playerPokeHp,
          enemyPokeHp: action.battle.enemyPokeHp,
          playerTurn: action.battle.playerTurn,
          actionLog: [...state.battle.actionLog, ...action.battle.actionLog.split('\n')],
        },
      };

    case 'EXIT_BATTLE':
      return { ...state, screen: 'game', battle: null };

    case 'UPDATE_TEAM':
      return { ...state, team: action.team, storage: action.storage };

    case 'UPDATE_ITEMS':
      return { ...state, items: action.items };

    case 'SET_LOADING':
      return { ...state, loading: action.loading };

    case 'SET_ERROR':
      return { ...state, error: action.error };

    case 'SET_NOTIFICATION':
      return { ...state, notification: action.notification };

    case 'RESET':
      return { ...initialState, sessionId: null, screen: 'start' };

    default:
      return state;
  }
}
