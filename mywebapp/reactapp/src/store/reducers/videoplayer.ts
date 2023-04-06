import { stat } from 'fs';
import { Action,PlayerStatus ,TrackModel} from '../../shared/types/BLL';
import ActionTypes from '../action-type';

export interface VideoPlayerState {
    queue: TrackModel[];
    // oldQueue: TrackModel[];
    // queueCursor: number | null;
    // queueOrigin: null | string;
    // repeat: Repeat;
    // shuffle: boolean;
    playerStatus: PlayerStatus;
    id:number
  }

  const initialState: VideoPlayerState = {
    queue: [], // Tracks to be played
    // oldQueue: [], // Queue backup (in case of shuffle)
    // queueCursor: null, // The cursor of the queue
    // queueOrigin: null, // URL of the queue when it was started
    // repeat: config.get('audioRepeat'), // the current repeat state (one, all, none)
    // shuffle: config.get('audioShuffle'), // If shuffle mode is enabled
    playerStatus: PlayerStatus.STOP, // Player status
    id:2
  };

export default (state = initialState, action: Action): VideoPlayerState => {
    switch (action.type) {
        case ActionTypes.MOVIE_ADD:
            {
                const queue = action.payload;
                return {
                    ...state,
                    queue
                };
            };
       
        case ActionTypes.TESTID:{
            const id=action.payload;
            return{
                ...state,
                id:state.id+1
            };
        };
            
    
        default:
            return initialState;
    }
};