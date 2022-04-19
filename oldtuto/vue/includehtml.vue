<script setup>
import { ref, onMounted } from 'vue'

const rawHtml = ref(`<span style="color: red">This should be red.</span>`);
</script>


<template>
  <p>On test: {{rawHtml}} </p>
  <p><span v-html="rawHtml"></span></p>
</template>