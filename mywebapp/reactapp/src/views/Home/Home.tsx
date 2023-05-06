import React, { useMemo } from 'react';
import { Outlet } from 'react-router';
import appStyles from '../../App.module.css';
import {PlayerExample} from '../../components/PlayerExample/PlayerExample';
import styles from './Home.module.css';


const Home:React.FC=()=>{

    const getHomeComponent=useMemo(()=>{
        return (
            <div>
                <p>ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABgQDTakQnseEupPOJf/t9q68uttMbpqsQL4PJ4M+Qn9HYrMSg4Sler7lTbI7kDmG2OcOwd0yGZYh1ARVjxaqj5A7GH11rap7pMAipgLvHG7vSjpYalwJd8JjGqZXzk2YeyLunhNKRivCh9yKCmp2D2vjmaaMUyPIRE/hXOQ7o4evvmA8JXZ3OmBsItWxznwf53Jm68zcao59xOyJ2MjupsIpmJm8miXQ3Pj7pnnvvveAjUmUU7Thz2Yd0rOFVAQ7KMOyxNGBpCIJT27WBtozYXq+zesgs3GF9/AZ6qax10cF01zaeE98TUG0QjixwqE9eoMzt/5PkimTElwdkOiuCWXaJaj8wgD9EtjaSgwmxTAgS2jVXvQ+L0w7ovG7kFbPld6k+TvVCb7gsxfw6j/5bLXvLO/iaE460wLJpyrNJXMk5M3w5JUqxt5fQqg0WIHdwOo4k/3Owf4HfwSGuvwv2NAMh66FBm5JihOmlsk3ozvNNuhwBHaeCLL3zS5zJhLRSRR8= michael@michael-CW65S
</p>
            </div>
            // <Outlet />

        );
    },[]);
    return <div className={`${appStyles.view} ${styles.viewHome}`}>{getHomeComponent}</div>;

}
export default Home;