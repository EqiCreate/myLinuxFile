import React from 'react';
import { connect } from 'react-redux';
import { Outlet } from 'react-router';
import { NavLink } from 'react-router-dom';
import { PlayerStatus, TrackModel } from '../../shared/types/BLL';
import { RootState } from '../../store/reducers';
import { Button, Navbar, Container, Row, Col } from 'react-bootstrap';

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
            <header>
                <NavLink to='/home' title='home'>
                <Button variant="primary">Home</Button>
                </NavLink>
                <NavLink to='/studio' title='studio'>
                <Button variant="primary">Studio</Button>
                </NavLink>
                <NavLink to='/settings' title='settings'>
                <Button variant="primary">Settings</Button>
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
