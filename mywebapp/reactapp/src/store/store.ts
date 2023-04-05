import { createLogger } from 'redux-logger';
import { configureStore, Store } from '@reduxjs/toolkit';
import { persistStore, persistReducer, createTransform } from 'redux-persist';
import storage from 'redux-persist/lib/storage';
import {VideoPlayerState} from './reducers/videoplayer';

import rootReducer, { RootState } from './reducers';
import { PlayerStatus } from '../shared/types/BLL';

const logger = createLogger({
    collapsed: true,
  });

  const playerStatusTransform = createTransform(
    (ibund: VideoPlayerState)=>{
        return {...ibund,
            playerStatus:ibund.playerStatus === PlayerStatus.PLAY ? PlayerStatus.PAUSE:ibund.playerStatus};
    },null,
    {whitelist:['player']}

);

  const persistConfig = {
    key: 'root',
    storage,
    blacklist: ['video'],
    version: 1,
    transforms: [playerStatusTransform],
  };

const persistedReducer = persistReducer(persistConfig, rootReducer);

const store: Store<RootState> = configureStore({ reducer: persistedReducer, middleware: [logger], devTools: true });
export default store;
