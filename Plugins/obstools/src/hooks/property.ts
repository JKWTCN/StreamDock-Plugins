import TabView from '@/components/tab-view.vue';
import { defineStore } from 'pinia';

// 属性钩子
export { TabView };
export const usePropertyStore = defineStore('propertyStore', () => {
  document.title = window.argv[3].plugin.uuid + ' - 属性检查器';

  // 监听数据
  const preventWatch = ref(false);
  const settings = ref(window.argv[4].payload.settings);
  watch(
    settings,
    () => {
      if (preventWatch.value) return;
      server.send(
        JSON.stringify({
          event: 'setSettings',
          context: window.argv[1],
          payload: settings.value
        })
      );
    },
    { deep: true }
  );

  // 连接软件
  const message = ref<StreamDock.Message>();
  const server = new WebSocket('ws://127.0.0.1:' + window.argv[0]);
  server.onopen = () => server.send(JSON.stringify({ event: window.argv[2], uuid: window.argv[1] }));
  server.onmessage = (e) => {
    message.value = JSON.parse(e.data)
    // console.log(JSON.parse(e.data));
    //这个是websocket收到消息我打印了一下
  };

  // 通知插件
  const sendToPlugin = (payload: any) => {
    server.send(
      JSON.stringify({
        event: 'sendToPlugin',
        action: window.argv[4].action,
        context: window.argv[1],
        payload
      })
    );
  };

  // 更改状态
  const setState = (state: number) => {
    server.send(
      JSON.stringify({
        event: 'setState',
        context: window.argv[4].context,
        payload: { state }
      })
    );
  };

  // 测试加的实际没必要调用上面有监听会自己发setSettings
  const setSettings = (settings: any) => {
    server.send(
      JSON.stringify({
        event: 'setSettings',
        context: window.argv[1],
        payload: settings
      })
    );
  };


  // 设置标题
  const setTitle = (title: string) => {
    server.send(
      JSON.stringify({
        event: 'setTitle',
        context: window.argv[4].context,
        payload: {
          title,
          target: 0
        }
      })
    );
  };

  const setGlobalSettings = (payload: any) => {
    server.send(
      JSON.stringify({
        event: 'setGlobalSettings',
        context: window.argv[1],
        payload
      })
    );
  };

  const getGlobalSettings = () => {
    server.send(
      JSON.stringify({
        event: 'getGlobalSettings',
        context: window.argv[1],
      })
    );
  };

  const getSettings = () => {
    server.send(
      JSON.stringify({
        event: 'getSettings',
        context: window.argv[1],
      })
    );
  };

  // 设置图片
  const setImage = (url: string) => {
    if (url.includes('data:')) {
      server.send(JSON.stringify({ event: 'setImage', context: window.argv[4].context, payload: { target: 0, image: url } }));
      return;
    }
    const image = new Image();
    image.src = url;
    image.onload = () => {
      const canvas = document.createElement('canvas');
      canvas.width = image.naturalWidth;
      canvas.height = image.naturalHeight;
      const ctx = canvas.getContext('2d');
      ctx.drawImage(image, 0, 0);
      server.send(JSON.stringify({ event: 'setImage', context: window.argv[4].context, payload: { target: 0, image: canvas.toDataURL('image/png') } }));
    };
  };

  // 用默认浏览器打开
  const openUrl = (url: string) => {
    server.send(
      JSON.stringify({
        event: 'openUrl',
        payload: { url }
      })
    );
  };

  return {
    message,
    preventWatch,
    settings,
    sendToPlugin,
    getGlobalSettings,
    setGlobalSettings,
    setState,
    setTitle,
    setSettings,
    getSettings,
    setImage,
    openUrl
  };
});

// !! 请勿更改此处 !!
export const useWatchEvent = (MessageEvents: StreamDock.ProperMessage) => {
  const property = usePropertyStore();
  const Events: StreamDock.ProperMessage = {
    didReceiveSettings(data) {
      property.preventWatch = true;
      property.settings = data.payload.settings;
      nextTick(() => {
        property.preventWatch = false;
      });
    }
  };
  watch(
    () => property.message,
    () => {
      if (!property.message) return;
      const data = JSON.parse(JSON.stringify(property.message));
      Events[property.message.event]?.(data);
      MessageEvents[property.message.event]?.(data);
    }
  );
};