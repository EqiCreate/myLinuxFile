import React, { useMemo } from 'react';
import appStyles from '../../App.module.css';
import styles from './Studio.module.css';
import { Player } from 'video-react';
import { useSelector } from 'react-redux';
import { RootState } from '../../store/reducers';
import * as StudioAction from '../../store/actions/ViedeoAction'
import { PlayerExample } from '../../components/PlayerExample/PlayerExample';

const Studio:React.FC=()=>{
    const studio = useSelector((state: RootState) => state.studio);
    var newTrack="";

    const TrackAdd=()=>{
        if(newTrack!=""){
            StudioAction.add({path:newTrack});
        }
    }
    const inputChange=(e:React.ChangeEvent<HTMLInputElement>)=>{
        newTrack=e.target.value;
    }

    const getStudioComponent=useMemo(()=>{
        if(studio.queue.length===0){
            return(
                <button>loading</button>
            );
        }
       
        return (
            <div>
                <button >{studio.queue[0].path}</button>
            </div>
          
        );
    },[studio.queue]);
    return <div className={`${appStyles.view} ${styles.viewLibrary}`}>
                <input  onChange={inputChange}></input>
                <button onClick={TrackAdd} >Add</button>
                <div className={`${styles.viewPlayer}`}>
                    <PlayerExample >
                            {/* <source src={studio.queue[studio.queue.length-1].path}/> */}
                    </PlayerExample>
                </div>
                <div>
                    <footer>dasdf</footer>
                </div>
        {getStudioComponent}</div>;

}
export default Studio;