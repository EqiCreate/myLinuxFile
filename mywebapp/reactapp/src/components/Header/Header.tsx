import React from 'react';
import { connect } from 'react-redux';
import { Outlet } from 'react-router';
import { NavLink } from 'react-router-dom';
import { PlayerStatus, TrackModel } from '../../shared/types/BLL';
import { RootState } from '../../store/reducers';
import { Button, Navbar, Container, Row, Col } from 'react-bootstrap';
import { FaHome, FaInfoCircle, FaCog,FaRocketchat } from 'react-icons/fa';
import styles from './Header.module.css';

interface Props {
    playerStatus: PlayerStatus;
    queue: TrackModel[];
  }

class Header extends React.Component<Props>{

    constructor(props:Props){
        super(props);
    }
    render(){
        return(
            <div>
            <header className={`${styles.viewHeader}`}  >
                <NavLink to='/home' title='home'>
                <Button variant="primary"><FaHome></FaHome></Button>
                </NavLink>
                <NavLink to='/chat' title='Chat'>
                <Button variant="primary"><FaRocketchat></FaRocketchat></Button>
                </NavLink>
                <NavLink to='/studio' title='studio'>
                <Button variant="primary"><FaInfoCircle/></Button>
                </NavLink>
                <NavLink to='/settings' title='settings'>
                <Button variant="primary"><FaCog/></Button>
                </NavLink>
            </header>
          {/* <Outlet></Outlet> */}

            </div>
            
        );
    };
}

const mapStateToProps = ({ studio }: RootState) => ({
    playerStatus: studio.playerStatus,
    queue:studio.queue
  });
export default connect(mapStateToProps)(Header);
