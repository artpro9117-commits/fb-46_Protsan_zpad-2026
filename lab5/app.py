import streamlit as st
import pandas as pd
import os
import plotly.express as px

st.set_page_config(layout="wide", page_title="Аналіз даних NOAA")

@st.cache_data
def load_data(folder_path="data"):
    province_mapping = {
        1: (22, 'Черкаська'), 2: (24, 'Чернігівська'), 3: (23, 'Чернівецька'),
        4: (25, 'АР Крим'), 5: (3, 'Дніпропетровська'), 6: (4, 'Донецька'),
        7: (8, 'Івано-Франківська'), 8: (19, 'Харківська'), 9: (20, 'Херсонська'),
        10: (21, 'Хмельницька'), 11: (9, 'Київська'), 12: (26, 'м. Київ'),
        13: (10, 'Кіровоградська'), 14: (11, 'Луганська'), 15: (12, 'Львівська'),
        16: (13, 'Миколаївська'), 17: (14, 'Одеська'), 18: (15, 'Полтавська'),
        19: (16, 'Рівненська'), 20: (27, 'м. Севастополь'), 21: (17, 'Сумська'),
        22: (18, 'Тернопільська'), 23: (6, 'Закарпатська'), 24: (1, 'Вінницька'),
        25: (2, 'Волинська'), 26: (7, 'Запорізька'), 27: (5, 'Житомирська')
    }
    
    all_dataframes = []
    if not os.path.exists(folder_path):
        return pd.DataFrame()

    for filename in os.listdir(folder_path):
        if filename.endswith(".csv"):
            old_id = int(filename.split('_')[2])
            filepath = os.path.join(folder_path, filename)
            df = pd.read_csv(filepath, header=1, index_col=False)
            
            df.columns = [col.replace('<br>', '').strip() for col in df.columns]
            expected_cols = ['year', 'week', 'SMN', 'SMT', 'VCI', 'TCI', 'VHI']
            df = df[[c for c in expected_cols if c in df.columns]]
            
            if not df.empty and 'year' in df.columns:
                df['year'] = df['year'].astype(str).replace({'<tt><pre>': '', '</pre></tt>': ''}, regex=True).str.strip()
                df['year'] = pd.to_numeric(df['year'], errors='coerce')
                df['week'] = pd.to_numeric(df['week'], errors='coerce')
                df = df.dropna(subset=['year', 'week'])
                df['year'] = df['year'].astype(int)
                df['week'] = df['week'].astype(int)
                
            df = df.replace(-1, pd.NA).dropna()
            new_id, prov_name = province_mapping.get(old_id, (old_id, "Невідомо"))
            df['area_id'] = new_id
            df['area_name'] = prov_name
            all_dataframes.append(df)
            
    if not all_dataframes:
        return pd.DataFrame()
        
    final_df = pd.concat(all_dataframes, ignore_index=True)
    final_df['Date'] = final_df['year'].astype(str) + " - Тиждень " + final_df['week'].astype(str)
    return final_df

df = load_data()

if df.empty:
    st.error("Дані не знайдено! Переконайтеся, що папка 'data' з CSV-файлами знаходиться поруч зі скриптом.")
    st.stop()

if 'resetting' not in st.session_state:
    st.session_state.resetting = False

def reset_filters():
    st.session_state.clear()

st.title("NOAA Дані: Аналіз вегетаційних індексів")

col_controls, col_content = st.columns([1, 3])

with col_controls:
    st.header("Налаштування")
    
    st.button("Скинути всі фільтри", on_click=reset_filters)
    
    metric = st.selectbox("Оберіть індекс:", ['VHI', 'VCI', 'TCI'], key='metric')
    regions_list = sorted(df['area_name'].unique())
    selected_region = st.selectbox("Оберіть область:", regions_list, key='region')
    
    min_year, max_year = int(df['year'].min()), int(df['year'].max())
    year_range = st.slider("Інтервал років:", min_value=min_year, max_value=max_year, value=(min_year, max_year), key='years')
    
    week_range = st.slider("Інтервал тижнів:", min_value=1, max_value=52, value=(1, 52), key='weeks')
    
    st.markdown("---")
    st.write("Сортування результатів:")
    sort_asc = st.checkbox("За зростанням", key='sort_asc')
    sort_desc = st.checkbox("За спаданням", key='sort_desc')

df_filtered = df[
    (df['area_name'] == selected_region) &
    (df['year'] >= year_range[0]) & (df['year'] <= year_range[1]) &
    (df['week'] >= week_range[0]) & (df['week'] <= week_range[1])
]

df_all_regions_time = df[
    (df['year'] >= year_range[0]) & (df['year'] <= year_range[1]) &
    (df['week'] >= week_range[0]) & (df['week'] <= week_range[1])
]

if sort_asc and sort_desc:
    st.warning("⚠️ Обрано обидва напрямки сортування. Дані виводяться у хронологічному порядку.")
elif sort_asc:
    df_filtered = df_filtered.sort_values(by=metric, ascending=True)
elif sort_desc:
    df_filtered = df_filtered.sort_values(by=metric, ascending=False)

with col_content:
    tab1, tab2, tab3 = st.tabs(["📄 Таблиця", "📈 Графік динаміки", "📊 Порівняння областей"])
    
    with tab1:
        st.subheader(f"Таблиця даних: {selected_region} область")
        st.dataframe(df_filtered[['year', 'week', 'SMN', 'SMT', 'VCI', 'TCI', 'VHI']], use_container_width=True)
        
    with tab2:
        st.subheader(f"Динаміка {metric} для {selected_region} області")
        if sort_asc or sort_desc:
            st.info("Графік вимкнено під час сортування за значенням (лінії втрачають хронологічний сенс). Вимкніть чекбокси сортування.")
        else:
            fig1 = px.line(df_filtered, x='Date', y=metric, 
                           title=f"Зміни {metric} з {year_range[0]} по {year_range[1]} роки",
                           labels={'Date': 'Хронологія (Рік - Тиждень)', metric: f'Значення {metric}'})
            st.plotly_chart(fig1, use_container_width=True)
            
    with tab3:
        st.subheader(f"Порівняння середнього {metric} серед усіх областей")
        comparison_df = df_all_regions_time.groupby('area_name')[metric].mean().reset_index()
        comparison_df = comparison_df.sort_values(by=metric, ascending=False)
        
        comparison_df['Колір'] = comparison_df['area_name'].apply(lambda x: 'Обрана область' if x == selected_region else 'Інші')
        
        fig2 = px.bar(comparison_df, x='area_name', y=metric, color='Колір',
                      color_discrete_map={'Обрана область': 'red', 'Інші': 'blue'},
                      title=f"Середнє значення {metric} ({year_range[0]}-{year_range[1]} рр., тижні {week_range[0]}-{week_range[1]})")
        fig2.update_layout(xaxis_title="Область", yaxis_title=f"Середній {metric}")
        st.plotly_chart(fig2, use_container_width=True)