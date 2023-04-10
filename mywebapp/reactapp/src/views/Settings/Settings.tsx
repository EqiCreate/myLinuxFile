import React from 'react';
import {  NavLink, Outlet } from 'react-router-dom';


import styles from './Settings.module.css';

const Settings: React.FC = () => {
  // const match = useMatch('/settings');

  return (
    <div className={`${styles.viewSettings}`}>
      <div className={styles.settings__nav}>
        <nav>
        <NavLink to='/settings/library'>
            Library
          </NavLink>
          <NavLink to='/settings/about'>
            About
          </NavLink>
        </nav>
         
      </div>
      <div className={styles.settings__content}>
        <Outlet />
      </div>
      {/* {match && <Navigate to='/settings/library' />} */}
    </div>
  );
};

export default Settings;
