import React, { useCallback, useEffect } from 'react';
import logo from './logo.svg';
import './App.css';
import 'video-react/dist/video-react.css'; // import css
import 'react-toastify/dist/ReactToastify.css';
import 'bootstrap/dist/css/bootstrap.min.css';
import 'slick-carousel/slick/slick.css';
import 'slick-carousel/slick/slick-theme.css';
import { Outlet, useNavigate } from 'react-router';
import styles from './App.module.css';
import Header from './components/Header/Header';
import Footer from './components/Footer/Footer';
import {  ToastContainer } from 'react-toastify';

type Props = {
  children?: React.ReactNode
};

const appMain: React.FC <Props> = ({children}) => {
  // const navigate = useNavigate();
  // useEffect(() => {
  //     // AppActions.init();
  //   }, [navigate]);
  return (
      <div className={`${styles.root}`} >
          
           <div className={`${styles.flexitem1}`}>
                {/* <header className={styles.headerTest}> */}
                  {/* <Header></Header> */}
                  <Footer></Footer>
                {/* </header> */}
           </div>
           <div className={`${styles.flexitem2}`}>
            <main className={styles.mainContent}>{children}</main>
           </div>
           <div className={`${styles.flexitem3}`}>
            <Header></Header>
           </div>
           <ToastContainer />
      </div>
  );
};
export default appMain;

// export default App;
