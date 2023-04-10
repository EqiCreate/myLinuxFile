import React, { useMemo } from 'react';
import { Outlet } from 'react-router';
import appStyles from '../../App.module.css';
import {PlayerExample} from '../../components/PlayerExample/PlayerExample';
import styles from './Home.module.css';


const Home:React.FC=()=>{

    const getHomeComponent=useMemo(()=>{
        return (
            <Outlet />

        );
    },[]);
    return <div className={`${appStyles.view} ${styles.viewHome}`}>{getHomeComponent}</div>;

}
export default Home;