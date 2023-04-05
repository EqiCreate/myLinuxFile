import { combineReducers } from "redux"
import app from './app';
import studio from './videoplayer';


export type RootState= ReturnType<typeof rootReducre>
 const rootReducre= combineReducers({
    app,
    studio
})

export default rootReducre;