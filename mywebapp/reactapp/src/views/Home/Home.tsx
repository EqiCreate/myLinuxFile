import React, { useMemo } from 'react';
import { Outlet } from 'react-router';
import appStyles from '../../App.module.css';
import {PlayerExample} from '../../components/PlayerExample/PlayerExample';
import styles from './Home.module.css';
import Carousel from 'react-bootstrap/Carousel';

const Home:React.FC=()=>{

    const getHomeComponent=useMemo(()=>{
        return (
<Carousel interval={5000} data-bs-theme="dark">
      <Carousel.Item>
        {/* <ExampleCarouselImage text="First slide" /> */}
        <img src="http://e.hiphotos.baidu.com/image/pic/item/a1ec08fa513d2697e542494057fbb2fb4316d81e.jpg"/>
        <Carousel.Caption>
          <h3>First slide label</h3>
          <p>Nulla vitae elit libero, a pharetra augue mollis interdum.</p>
        </Carousel.Caption>
      </Carousel.Item>
      <Carousel.Item>
        {/* <ExampleCarouselImage text="Second slide" /> */}
        <img src="http://c.hiphotos.baidu.com/image/pic/item/30adcbef76094b36de8a2fe5a1cc7cd98d109d99.jpg"/>
        <Carousel.Caption>
          <h3>Second slide label</h3>
          <p>Lorem ipsum dolor sit amet, consectetur adipiscing elit.</p>
        </Carousel.Caption>
      </Carousel.Item>
      <Carousel.Item>
        {/* <ExampleCarouselImage text="Third slide" /> */}
        <img src="http://h.hiphotos.baidu.com/image/pic/item/7c1ed21b0ef41bd5f2c2a9e953da81cb39db3d1d.jpg"/>
        <Carousel.Caption>
          <h3>Third slide label</h3>
          <p>
            Praesent commodo cursus magna, vel scelerisque nisl consectetur.
          </p>
        </Carousel.Caption>
      </Carousel.Item>
    </Carousel>
        );
    },[]);
    return <div className={`${appStyles.view} ${styles.viewHome}`}>{getHomeComponent}</div>;

}
export default Home;