import store from "../store";
import types from '../action-type';
import { Track } from "../../shared/types/BLL";

export const add =(newTrack:Track):void=>{
    let oldqueue= store.getState().studio.queue;
    // let newqueue=[...oldqueue,newTrack];
    let newqueue=[newTrack];

    store.dispatch({
        type:types.MOVIE_ADD,
        payload:newqueue
    });
};
export const testid=(id:number):void =>{
    let oldid=store.getState().studio.id;
    let newid=oldid++;
    store.dispatch({
        type:types.TESTID,
        payload:newid
    });
};