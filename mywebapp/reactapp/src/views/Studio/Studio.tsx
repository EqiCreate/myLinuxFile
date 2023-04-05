import React, { useMemo } from 'react';
import appStyles from '../../App.module.css';
import styles from './Studio.module.css';
import { Player } from 'video-react';
import { useSelector } from 'react-redux';
import { RootState } from '../../store/reducers';
import * as StudioAction from '../../store/actions/ViedeoAction'

const Studio:React.FC=()=>{
    const studio = useSelector((state: RootState) => state.studio);
    const TrackAdd=()=>{
        StudioAction.add({path:"dddd"});
    }
    const getStudioComponent=useMemo(()=>{
        if(studio.queue.length===0){
            return(
                <button>loading</button>
            );
        }
       
        return (
            <div>
               
                     {/* <Player> */}
                    {/* <source src="https://media.w3.org/2010/05/sintel/trailer_hd.mp4" /> */}
                    <button >{studio.queue[0].path}</button>

                 {/* </Player> */}
                
            </div>
          
        );
    },[studio.queue]);
    return <div className={`${appStyles.view} ${styles.viewLibrary}`}>
                <button onClick={TrackAdd} >Add</button>
        
        {getStudioComponent}</div>;

}
export default Studio;