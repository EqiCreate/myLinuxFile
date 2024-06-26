import React, { useMemo } from 'react';
import { Outlet } from 'react-router';
import appStyles from '../../App.module.css';
import {PlayerExample} from '../../components/PlayerExample/PlayerExample';
import styles from './Home.module.css';
import Carousel from 'react-bootstrap/Carousel';
import Card from 'react-bootstrap/Card';
import Button from 'react-bootstrap/Button';
import SearchInput, {createFilter} from 'react-search-input';
import {  toast } from 'react-toastify';
import Slider from 'react-slick';
import { List } from 'react-virtualized';
import { NavLink } from 'react-router-dom';
const Home:React.FC=()=>{

  const settings = {
    infinite: false, // 循环滚动
    slidesToShow: 3, // 每次显示3个子控件，根据需要调整
    slidesToScroll: 1, // 每次滚动1个子控件，根据需要调整
    // 其他配置选项可以根据需要添加
  };

  const searchUpdated=()=>{
   
  }
  const onCardClick=()=>{
    toast.success('这是一个成功通知！');
  }
  const data = [
    { title: 'Item 1', imageUrl: 'url1.jpg',id:1 },
    { title: 'Item 2', imageUrl: 'url2.jpg',id:2 },
    { title: 'Item 3', imageUrl: 'url3.jpg',id:3 },

    // ...更多数据
  ];
  const rowRenderer = ({ index, key, style }:any) => {
    const item = data[index];

  return (
    <NavLink to={`/item/${item.id}`}>
    {/* <div key={key} style={style} className="list-item" >
      <img src={item.imageUrl} alt={item.title} className="list-item-image" />
      <span className="list-item-title">{item.title}</span> */}
    {/* </div> */}
    </NavLink>
   
  );

  }
    const getHomeComponent=useMemo(()=>{
        return (
          <div >
            <div style={{height:'80px'}}>
              <SearchInput style={{display:'flex', width: 'calc(100% - 20px)',height:'40px',margin:'10px auto',border:'2px solid blue'}} onChange={searchUpdated} />
            </div>
          <Carousel interval={5000} data-bs-theme="dark" className={`${styles.viewCarousel}`}>
          <Carousel.Item>
            <img src="logo192.png"/>
            <Carousel.Caption>
              <h3>First slide label</h3>
              <p>Nulla vitae elit libero, a pharetra augue mollis interdum.</p>
            </Carousel.Caption>
          </Carousel.Item>
          <Carousel.Item  >
            <img src="logo512.png"/>
            <Carousel.Caption>
              <h3>Second slide label</h3>
              <p>Lorem ipsum dolor sit amet, consectetur adipiscing elit.</p>
            </Carousel.Caption>
          </Carousel.Item>
          <Carousel.Item >
            <img src="model.png"/>
            <Carousel.Caption>
              <h3>Third slide label</h3>
              <p>
                Praesent commodo cursus magna, vel scelerisque nisl consectetur.
              </p>
            </Carousel.Caption>
          </Carousel.Item>
        </Carousel>
        
        <Slider {...settings}>
          <Card className={`${styles.viewCardContainer1}`} onClick={onCardClick}>
          <Card.Img className={`${styles.viewCardImgEx}`}  variant="top" src="logo192.png" />
          <Card.Body >
            <Card.Text>
                DEBUG
            </Card.Text>
          </Card.Body>
          </Card>
          <Card className={`${styles.viewCardContainer1}`}>
          <Card.Img className={`${styles.viewCardImgEx}`}  variant="top" src="logo192.png" />
          <Card.Body >
            <Card.Text>
                DEBUG2
            </Card.Text>
          </Card.Body>
          </Card>
          <Card className={`${styles.viewCardContainer1}`}>
          <Card.Img className={`${styles.viewCardImgEx}`}  variant="top" src="logo192.png" />
          <Card.Body >
            <Card.Text>
                DEBUG3
            </Card.Text>
          </Card.Body>
          </Card>
          <Card className={`${styles.viewCardContainer1}`}>
          <Card.Img className={`${styles.viewCardImgEx}`}  variant="top" src="logo192.png" />
          <Card.Body >
            <Card.Text>
                DEBUG4
            </Card.Text>
          </Card.Body>
          </Card>
        </Slider>
        <List
          width={400} // 列表宽度
          border={'2px solid blue'}
          height={150} // 列表高度
          rowCount={data.length} // 数据行数
          rowHeight={50} // 每行高度
          rowRenderer={rowRenderer} // 渲染行的方法
          />
          </div>
     
        );
    },[]);
    return <div className={`${styles.viewHome}`}>{getHomeComponent}</div>;
}
export default Home;