from pylab import *
import matplotlib as mpl
mpl.rcParams['font.sans-serif'] = ['SimHei']

plt.figure(figsize=(8,6))
plt.plot(losses['train'], label='训练损失')
plt.legend()
plt.xlabel("批次")
plt.ylabel("损失")
_ = plt.ylim()

plt.figure(figsize=(8,6))
plt.plot(losses['test'], label='测试损失')
plt.legend()
plt.xlabel("批次")
plt.ylabel("损失")
_ = plt.ylim()
    
def get_tensors(loaded_graph):
    '''
    使用 get_tensor_by_name() 函数从 loaded_graph 中获取tensors
    '''
    uid = loaded_graph.get_tensor_by_name("uid:0")
    user_gender = loaded_graph.get_tensor_by_name("user_gender:0")
    user_age = loaded_graph.get_tensor_by_name("user_age:0")
    user_job = loaded_graph.get_tensor_by_name("user_job:0")
    movie_id = loaded_graph.get_tensor_by_name("movie_id:0")
    movie_categories = loaded_graph.get_tensor_by_name("movie_categories:0")
    movie_titles = loaded_graph.get_tensor_by_name("movie_titles:0")
    targets = loaded_graph.get_tensor_by_name("targets:0")
    dropout_keep_prob = loaded_graph.get_tensor_by_name("dropout_keep_prob:0")
    lr = loaded_graph.get_tensor_by_name("LearningRate:0")
    inference = loaded_graph.get_tensor_by_name("inference/ExpandDims:0")
    movie_combine_layer_flat = loaded_graph.get_tensor_by_name("movie_fc/Reshape:0")
    user_combine_layer_flat = loaded_graph.get_tensor_by_name("user_fc/Reshape:0")
    return uid, user_gender, user_age, user_job, movie_id, movie_categories, movie_titles, targets, lr, dropout_keep_prob, inference, movie_combine_layer_flat, user_combine_layer_flat

loaded_graph = tf.Graph()
movie_matrics = []
with tf.Session(graph=loaded_graph) as sess:
    # 载入保存好的模型
    loader = tf.train.import_meta_graph(load_dir + '.meta')
    loader.restore(sess, load_dir)

    # 调用函数拿到 tensors
    uid, user_gender, user_age, user_job, movie_id, movie_categories, movie_titles, targets, lr, dropout_keep_prob, _, movie_combine_layer_flat, __ = get_tensors(loaded_graph)  #loaded_graph

    for item in movies.values:
        categories = np.zeros([1, 18])
        categories[0] = item.take(2)

        titles = np.zeros([1, sentences_size])
        titles[0] = item.take(1)

        feed = {
            movie_id: np.reshape(item.take(0), [1, 1]),
            movie_categories: categories,
            movie_titles: titles,
            dropout_keep_prob: 1}

        movie_combine_layer_flat_val = sess.run([movie_combine_layer_flat], feed)
        # 添加进一个list中
        movie_matrics.append(movie_combine_layer_flat_val)
# 保存成 .p 文件
pickle.dump((np.array(movie_matrics).reshape(-1, 200)), open('movie_matrics.p', 'wb'))
# 读取文件
movie_matrics = pickle.load(open('movie_matrics.p', mode='rb'))

loaded_graph = tf.Graph()
users_matrics = []
with tf.Session(graph=loaded_graph) as sess:
    # 载入保存好的模型
    loader = tf.train.import_meta_graph(load_dir + '.meta')
    loader.restore(sess, load_dir)

    # 调用函数拿到 tensors
    uid, user_gender, user_age, user_job, movie_id, movie_categories, movie_titles, targets, lr, dropout_keep_prob, _, __,user_combine_layer_flat = get_tensors(loaded_graph)  #loaded_graph

    for item in users.values:

        feed = {
            uid: np.reshape(item.take(0), [1, 1]),
            user_gender: np.reshape(item.take(1), [1, 1]),
            user_age: np.reshape(item.take(2), [1, 1]),
            user_job: np.reshape(item.take(3), [1, 1]),
            dropout_keep_prob: 1}

        user_combine_layer_flat_val = sess.run([user_combine_layer_flat], feed)  
        # 添加进一个list中
        users_matrics.append(user_combine_layer_flat_val)
# 保存成 .p 文件
pickle.dump((np.array(users_matrics).reshape(-1, 200)), open('users_matrics.p', 'wb'))
# 读取文件
users_matrics = pickle.load(open('users_matrics.p', mode='rb'))

def recommend_same_type_movie(movie_id_val, top_k = 20):
    
    loaded_graph = tf.Graph()  #
    with tf.Session(graph=loaded_graph) as sess:  #
        # Load saved model
        loader = tf.train.import_meta_graph(load_dir + '.meta')
        loader.restore(sess, load_dir)
        
        norm_movie_matrics = tf.sqrt(tf.reduce_sum(tf.square(movie_matrics), 1, keep_dims=True))
        normalized_movie_matrics = movie_matrics / norm_movie_matrics

        #推荐同类型的电影
        probs_embeddings = (movie_matrics[movieid2idx[movie_id_val]]).reshape([1, 200])
        probs_similarity = tf.matmul(probs_embeddings, tf.transpose(normalized_movie_matrics))
        sim = (probs_similarity.eval())
    #     results = (-sim[0]).argsort()[0:top_k]
    #     print(results)
        
        print("您看的电影是：{}".format(movies_orig[movieid2idx[movie_id_val]]))
        print("以下是给您的推荐：")
        p = np.squeeze(sim)
        p[np.argsort(p)[:-top_k]] = 0
        p = p / np.sum(p)
        results = set()
        while len(results) != 5:
            c = np.random.choice(3883, 1, p=p)[0]
            results.add(c)
        for val in (results):
            print(val)
            print(movies_orig[val])
        
        return results

import random

def recommend_other_favorite_movie(movie_id_val, top_k = 20):
    
    # 推荐看过同一个的电影的人喜欢的电影
    print("您看的电影是：{}".format(movies_orig[movieid2idx[movie_id_val]]))
    # 根据电影寻找相似的人
    probs_movie_embeddings = (movie_matrics[movieid2idx[movie_id_val]]).reshape([1, 200])
    probs_movie_embeddings = probs_movie_embeddings/np.sqrt(np.sum(np.square(probs_movie_embeddings)))
    
    norm_users_matrics = np.sqrt(np.sum(np.square(users_matrics),axis=1)).reshape(6040,1)
    normalized_users_matrics = users_matrics/norm_users_matrics
    probs_user_favorite_similarity = np.matmul(probs_movie_embeddings, np.transpose(normalized_users_matrics))
    
    favorite_user_id = np.argsort(probs_user_favorite_similarity)[0][-top_k:]
    print("喜欢看这个电影的人是：{}".format(users_orig[favorite_user_id-1]))
    # 他们喜欢什么样的电影
    probs_users_embeddings = (users_matrics[favorite_user_id-1]).reshape([-1, 200])
    probs_users_embeddings = probs_users_embeddings/np.sqrt(np.sum(np.square(probs_users_embeddings)))
    
    norm_movie_matrics = np.sqrt(np.sum(np.square(movie_matrics),axis=1)).reshape(3883,1)
    normalized_movie_matrics = movie_matrics/norm_movie_matrics
    probs_similarity = np.matmul(probs_users_embeddings, np.transpose(normalized_movie_matrics))
    p = np.argmax(probs_similarity, 1)
    print("喜欢看这个电影的人还喜欢看：")
    
    results = set()
    while len(results) != 5:
        c = p[random.randrange(top_k)]
        results.add(c)
    for val in (results):
        print(val)
        print(movies_orig[val])

    return results