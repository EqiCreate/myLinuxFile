import React, { useMemo } from 'react';
import appStyles from '../../App.module.css';
import {Form1,PlayerExample} from '../../components/PlayerExample/PlayerExample';
import styles from './Home.module.css';


const Home:React.FC=()=>{

    const getHomeComponent=useMemo(()=>{
        return (
            <div>
            </div>
        );
    },[]);
    return <div className={`${appStyles.view} ${styles.viewHome}`}>{getHomeComponent}</div>;

}
export default Home;