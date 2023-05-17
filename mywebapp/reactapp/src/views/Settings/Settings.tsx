import React from 'react';
import {  NavLink, Outlet } from 'react-router-dom';
import { ToastContainer } from 'react-toastify';


import styles from './Settings.module.css';

const Settings: React.FC = () => {
  // const match = useMatch('/settings');

  return (
    <div className={`${styles.viewSettings}`}>
      <div className={styles.settings__nav}>
        {/* <nav > */}
        <NavLink to='/settings/upload'>
            upload
          </NavLink>
          <NavLink to='/settings/about'>
            About
          </NavLink>
          <NavLink to='/settings/download'>
            Stored Files
          </NavLink>
        {/* </nav> */}
         
      </div>
      
      <div className={styles.settings__content}>
        <Outlet />
      </div>
      <ToastContainer />

      {/* {match && <Navigate to='/settings/library' />} */}
    </div>
  );
};

export default Settings;
