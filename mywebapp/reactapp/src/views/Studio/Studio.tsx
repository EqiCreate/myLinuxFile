import React, { useEffect, useMemo, useState } from 'react';
import appStyles from '../../App.module.css';
import styles from './Studio.module.css';
import { useSelector } from 'react-redux';
import { RootState } from '../../store/reducers';
import * as StudioAction from '../../store/actions/ViedeoAction'
import { PlayerExample } from '../../components/PlayerExample/PlayerExample';
import { Page } from '../../components/Page/Page';
import { Video } from '../../shared/types/BLL';
import { raw_data_videos } from '../../raw-data-videos';

export interface VideoMeta{
    name:nameV,
    cca2:string
}
interface nameV{
    common:string
}

export const Studio:React.FC=()=>{
    const studio = useSelector((state: RootState) => state.studio);
    const [data, setData] = useState<VideoMeta[]|null>(null);
    useEffect(()=>{
        fetch('http://192.168.3.61:7268/FileUpload/top10')
        .then(response => response.json())
        .then(data => setData(data));
    },[]);
    var newTrack="";
    // const[id,setid] =useState(1);
    // var id :number =0;
    const TrackAdd=()=>{
        if(newTrack!=""){
            // StudioAction.add({path:newTrack});
            StudioAction.testid(1);

        }
    }
    const inputChange=(e:React.ChangeEvent<HTMLInputElement>)=>{
        newTrack=e.target.value;
    }
    const getVideosFromRawData = (raw: any[]): Video[] => {
        let commbinedArr=raw;
        if(data ==null)
        {

        }
        else{
            let filterData=data.filter(v=>v.name.common.endsWith("mp4") ||v.name.common.endsWith("MOV"));
            commbinedArr=[...filterData,...raw];
        }
        return commbinedArr.map((value: any) => (
        {
          __typename: "video",
          name: String(value.name.common),
          Url: `${String(value.cca2)}`
        }
        ));
      };
   
    const getStudioComponent=useMemo(()=>{
        // if(studio.queue.length===0){
        //     return(
        //         <button>loading</button>
        //     );
        // }
       
        return (
            <div>
                  <div className={`${styles.viewPlayer}`}>
                    <PlayerExample  playerSource={studio.queue}/>
                </div>
                {/* <button >id== {studio.id} </button> */}

                {/* <button >{studio.queue[studio.queue.length-1].path} {studio.id} {studio.queue.length}</button> */}
            </div>
          
        );
    },[studio.queue]);
    return <div className={`${appStyles.view} ${styles.viewLibrary}`}>
                {/* <input  onChange={inputChange}></input> */}
                {/* <button onClick={TrackAdd} >Add</button> */}
                <Page videos={getVideosFromRawData(raw_data_videos)}></Page>
                <div>
                    {getStudioComponent}
                </div>
                
        </div>;

}
// export  Studio;