import { Action } from "../../shared/types/BLL";
import ActionTypes from "../action-type";

export type AppState = null;
const initialState = null;
export default (state= initialState,action:Action)=>{
    switch(action.type)
    {
        case ActionTypes.REFRESH_CONFIG:
            return state;
        default :
            return state;
    }

};